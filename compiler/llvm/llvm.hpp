#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
#include "../parser/ast.hpp"
#include "../parser/parser.hpp"
#include "../parser/symbols.hpp"

#include "./gen.hpp"

std::string generateLLVMCode(std::vector<pNode> nodes, Symbols symbols, bool func);

int var = 0;

struct LLVMSym {
    std::string name;
    std::string type;
    int index;
};

std::vector<int> stack_index;
std::vector<int>  func_index;

struct LLVM {
    static std::string toStr(Symbol type);
    static int toSize(Symbol type);

    static std::string lambda(Symbols& sym, pNode declaration, pNode lambda);

    static std::string store(pDeclaration decl, Symbols& sym, int src, int dst);

    static std::tuple<std::string, int> alloca(Symbol sym);
    static std::tuple<std::string, int> expr(std::string expect, Symbols& sym, std::shared_ptr<pNode>& data);
};

std::string LLVM::toStr(Symbol type) {
    if( type.kind == TYPE ) {
        if( type.radical == i8 )        return "i8";
        else if( type.radical == i16 )  return "i16";
        else if( type.radical == i32 )  return "i32";
        else if( type.radical == i64 )  return "i64";
        else if( type.radical == i128 ) return "i128";
        else if( type.radical == i256 ) return "i256";
        else return "ptr";
    } else return "void";
}

int LLVM::toSize(Symbol type) {
    if( type.kind == TYPE ) {
        if( type.radical == i8 )        return 1;
        else if( type.radical == i16 )  return 2;
        else if( type.radical == i32 )  return 4;
        else if( type.radical == i64 )  return 8;
        else if( type.radical == i128 ) return 16;
        else if( type.radical == i256 ) return 32;
        else return 8;
    } else return -1;
}

std::string LLVM::store(pDeclaration decl, Symbols& sym, int src, int dst) {
    std::stringstream ss;
    ss << "store " << toStr(decl.type) << " %" << src << ", " << "ptr" << " %" << dst << ", align " << toSize(decl.type) << "\n";
    return ss.str();
}

std::tuple<std::string, int> LLVM::alloca(Symbol sym) {
    std::stringstream ss;
    ss << "%" << var++ << " = " << "alloca " << toStr(sym) << ", align " << toSize(sym) << "\n";

    return { ss.str(), var - 1 };
}

std::tuple<std::string, int> LLVM::expr(std::string expect, Symbols& sym, std::shared_ptr<pNode>& data) {
    std::stringstream ss;

    pExpression expr = std::get<pExpression>(data->values);

    auto is_integer_type = [&](const std::string& type) {
        Symbol symbol = sym.get(type);
        return symbol.radical <= 32 && symbol.radical > 0;
    };

    auto is_float_type = [&](const std::string& type) {
        Symbol symbol = sym.get(type);
        return symbol.radical == 1000 || symbol.radical == 1001;
    };

    auto is_string_type = [&](const std::string& type) {
        Symbol symbol = sym.get(type);
        return symbol.radical == 2000; // STRING type
    };

    auto transform = [&](std::string op) {
        if (is_float_type(expect)) {
            if (op == "+") return "fadd";
            if (op == "-") return "fsub";
            if (op == "*") return "fmul";
            if (op == "/") return "fdiv";
        } else if (is_integer_type(expect)) {
            if (op == "+") return "add";
            if (op == "-") return "sub";
            if (op == "*") return "mul";
            if (op == "/") return "sdiv";
        } else if (is_string_type(expect) && op == "+") {
            return "string_concat";
        }
        CompilerOutputs::Fatal("Unknown operator: " + op + " for type " + expect);
        return "";
    };

    // Função para criar uma constante string no LLVM
    auto create_string_constant = [&](const std::string& str) -> std::tuple<std::string, std::string, int> {
        std::stringstream node_ss;

        // Cria o nome da constante global
        std::string global_name = "@.str." + std::to_string(var++);

        // Escapa caracteres especiais para LLVM
        std::string escaped_str;
        for (char c : str) {
            if (c == '\n') escaped_str += "\\0A";
            else if (c == '\t') escaped_str += "\\09";
            else if (c == '\"') escaped_str += "\\22";
            else if (c == '\\') escaped_str += "\\5C";
            else if (c < 32 || c > 126) {
                // Caracteres não-printáveis como hexadecimal
                char hex[5];
                snprintf(hex, sizeof(hex), "\\%02X", (unsigned char)c);
                escaped_str += hex;
            } else {
                escaped_str += c;
            }
        }

        // Declara a constante global
        node_ss << global_name << " = private unnamed_addr constant [" << (str.length() + 1) << " x i8] c\"" << escaped_str << "\\00\"\n";

        // Aloca espaço para o ponteiro
        std::string alloca_reg = "%" + std::to_string(var++);
        node_ss << alloca_reg << " = alloca [" << (str.length() + 1) << " x i8], align 1\n";

        // Copia a string constante para a memória alocada
        std::string copy_reg = "%" + std::to_string(var++);
        node_ss << copy_reg << " = bitcast ptr " << alloca_reg << " to ptr\n";

        std::string memcpy_reg = "%" + std::to_string(var++);
        node_ss << "call void @llvm.memcpy.p0.p0.i64(ptr " << copy_reg << ", ptr " << global_name
                << ", i64 " << (str.length() + 1) << ", i1 false)\n";

        return {node_ss.str(), alloca_reg, var};
    };

    // Função para concatenar duas strings
    auto concat_strings = [&](const std::string& left_str, const std::string& right_str) -> std::tuple<std::string, std::string, int> {
        std::stringstream node_ss;

        // Calcula tamanhos
        int left_len = left_str.length();
        int right_len = right_str.length();
        int total_len = left_len + right_len;

        // Aloca espaço para a string resultante
        std::string result_alloca = "%" + std::to_string(var++);
        node_ss << result_alloca << " = alloca [" << (total_len + 1) << " x i8], align 1\n";

        // Copia primeira string
        std::string dest_ptr1 = "%" + std::to_string(var++);
        node_ss << dest_ptr1 << " = bitcast ptr " << result_alloca << " to ptr\n";

        std::string left_global = "@.str.left." + std::to_string(var++);
        node_ss << left_global << " = private unnamed_addr constant [" << (left_len + 1) << " x i8] c\"";
        for (char c : left_str) {
            if (c == '\n') node_ss << "\\0A";
            else if (c == '\t') node_ss << "\\09";
            else if (c == '\"') node_ss << "\\22";
            else if (c == '\\') node_ss << "\\5C";
            else node_ss << c;
        }
        node_ss << "\\00\"\n";

        node_ss << "call void @llvm.memcpy.p0.p0.i64(ptr " << dest_ptr1 << ", ptr " << left_global
                << ", i64 " << left_len << ", i1 false)\n";

        // Copia segunda string após a primeira
        std::string dest_ptr2 = "%" + std::to_string(var++);
        node_ss << dest_ptr2 << " = getelementptr inbounds [" << (total_len + 1) << " x i8], ptr "
                << result_alloca << ", i64 0, i64 " << left_len << "\n";

        std::string right_global = "@.str.right." + std::to_string(var++);
        node_ss << right_global << " = private unnamed_addr constant [" << (right_len + 1) << " x i8] c\"";
        for (char c : right_str) {
            if (c == '\n') node_ss << "\\0A";
            else if (c == '\t') node_ss << "\\09";
            else if (c == '\"') node_ss << "\\22";
            else if (c == '\\') node_ss << "\\5C";
            else node_ss << c;
        }
        node_ss << "\\00\"\n";

        node_ss << "call void @llvm.memcpy.p0.p0.i64(ptr " << dest_ptr2 << ", ptr " << right_global
                << ", i64 " << (right_len + 1) << ", i1 false)\n";

        return {node_ss.str(), result_alloca, var};
    };

    // Função auxiliar para avaliar um nó da expressão e retornar o registrador
    auto evaluate_node = [&](const auto& node, const std::string& type) -> std::tuple<std::string, std::string, int> {
        std::stringstream node_ss;
        std::string result_value;

        if (std::holds_alternative<std::string>(node)) {
            std::string id = std::get<std::string>(node);
            Symbol symbol = sym.get(id);

            if (symbol.kind == SymKind::VARIABLE || symbol.kind == SymKind::CONSTANT) {
                std::string symbol_type = toStr(symbol);
                if (symbol_type != type) {
                    CompilerOutputs::Fatal("Type mismatch: expected " + type + ", got " + symbol_type);
                }

                if (symbol.kind == SymKind::CONSTANT) {
                    // Se é uma constante string, cria a constante
                    if (is_string_type(type)) {
                        auto [const_code, const_reg, new_var] = create_string_constant(id);
                        var = new_var;
                        return {const_code, const_reg, var};
                    }
                    return {"", id, var}; // Constante, não gera código
                } else {
                    if (is_string_type(type)) {
                        // Para variáveis string, já é um ponteiro para o array
                        return {"", "%" + id, var};
                    } else {
                        // Carrega variável da memória
                        std::string load_reg = "%" + std::to_string(var++);
                        node_ss << load_reg << " = load " << type << ", ptr %" << id << ", align " << toSize(symbol) << "\n";
                        return {node_ss.str(), load_reg, var};
                    }
                }
            }
        }
        else if (std::holds_alternative<double>(node)) {
            double value = std::get<double>(node);
            if (is_integer_type(type)) {
                if (value != std::floor(value)) {
                    CompilerOutputs::Fatal("Type mismatch! Expected integer type (" + type + "), but got float64 value " + std::to_string(value));
                }
                return {"", std::to_string(static_cast<int64_t>(value)), var};
            } else if (is_float_type(type)) {
                std::stringstream num_ss;
                num_ss << std::fixed << std::setprecision(10) << value;
                std::string num_str = num_ss.str();
                num_str.erase(num_str.find_last_not_of('0') + 1);
                if (num_str.back() == '.') num_str.pop_back();
                return {"", num_str, var};
            } else {
                CompilerOutputs::Fatal("Unsupported type " + type + " for numeric expressions");
            }
        }
        else if (std::holds_alternative<std::shared_ptr<pNode>>(node)) {
            auto sub_node = std::get<std::shared_ptr<pNode>>(node);
            auto [sub_code, sub_var] = LLVM::expr(type, sym, sub_node);
            std::string sub_reg = "%" + std::to_string(sub_var);
            return {sub_code, sub_reg, var};
        }

        CompilerOutputs::Fatal("Invalid expression node");
        return {"", "", var};
    };

    // Se não há operador, avalia apenas o lado esquerdo
    if (expr.op.empty()) {
        auto [code, value, new_var] = evaluate_node(expr.left, expect);
        var = new_var;
        return {code, var - 1};
    }

    // Se é concatenação de strings
    if (is_string_type(expect) && expr.op == "+") {
        // Avalia o lado esquerdo como string
        auto [left_code, left_value, left_var] = evaluate_node(expr.left, expect);
        var = left_var;

        // Avalia o lado direito como string
        auto [right_code, right_value, right_var] = evaluate_node(expr.right, expect);
        var = right_var;

        ss << left_code << right_code;

        // Concatena as strings
        auto [concat_code, concat_reg, concat_var] = concat_strings(left_value, right_value);
        var = concat_var;
        ss << concat_code;

        return {ss.str(), var - 1};
    }

    // Para outros operadores (numéricos)
    // Avalia o lado esquerdo
    auto [left_code, left_value, left_var] = evaluate_node(expr.left, expect);
    var = left_var;
    std::string left_operand = left_value;

    // Adiciona código do lado esquerdo
    if (!left_code.empty()) {
        ss << left_code;
    }

    if (left_operand.find("%") != 0 && !is_string_type(expect)) {
        std::string temp_reg = "%" + std::to_string(var++);
        if (is_integer_type(expect)) {
            ss << temp_reg << " = add " << expect << " " << left_operand << ", 0\n";
        } else if (is_float_type(expect)) {
            ss << temp_reg << " = fadd " << expect << " " << left_operand << ", 0.0\n";
        } else {
            CompilerOutputs::Fatal("Unsupported type for operand: " + expect);
        }
        left_operand = temp_reg;
    }

    // Avalia o lado direito
    auto [right_code, right_value, right_var] = evaluate_node(expr.right, expect);
    var = right_var;
    std::string right_operand = right_value;

    // Adiciona código do lado direito
    if (!right_code.empty()) {
        ss << right_code;
    }

    // Se o operando direito não é um registrador, cria um
    if (right_operand.find("%") != 0 && !is_string_type(expect)) {
        std::string temp_reg = "%" + std::to_string(var++);
        if (is_integer_type(expect)) {
            ss << temp_reg << " = add " << expect << " " << right_operand << ", 0\n";
        } else if (is_float_type(expect)) {
            ss << temp_reg << " = fadd " << expect << " " << right_operand << ", 0.0\n";
        } else {
            CompilerOutputs::Fatal("Unsupported type for operand: " + expect);
        }
        right_operand = temp_reg;
    }

    // Gera a operação final
    std::string result_reg = "%" + std::to_string(var++);
    std::string operation = transform(expr.op);

    ss << result_reg << " = " << operation << " " << expect << " " << left_operand << ", " << right_operand << "\n";

    return {ss.str(), var - 1};
}

std::string LLVM::lambda(Symbols& sym, pNode declaration, pNode lambda) {
    var = 0;

    std::stringstream ss;

    pDeclaration decl = std::get<pDeclaration>(declaration.values);
    pLambda func = std::get<pLambda>(lambda.values);
    std::string type = toStr(decl.type);

    ss << "define " << type << " @" << decl.name << "(";

    bool first = true;
    for( auto param : func.args ) {
        if(! first ) ss << ", ";

        pDeclaration arg = std::get<pDeclaration>(param.values);
        ss << toStr(arg.type) << " %" << var++ << "";
        first = false;
    }

    var++;

    ss << ") {\n";
    ss << "entry:\n";

    std::vector<pNode> stmt;
    Parser::checkSyntax(sym, &stmt, std::get<std::vector<pContext>>(func.body.content), false);
    ss << generateLLVMCode(stmt, sym, true);

    ss << "ret i32 0\n";
    ss << "}\n\n";

    var = 0;

    return ss.str();
}
