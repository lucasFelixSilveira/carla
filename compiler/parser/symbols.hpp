// Isto representa a tabela de simbolos do compilador

#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <functional>
#include <iostream>
#include "../compiler_outputs.hpp"

#include "llvm_rtypes.hpp"

enum SymKind {
    VARIABLE,
    TYPE,
    FUNCTION,
    CONSTANT,
    MODULE
};

enum SymVariant {
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_PRIMITIVE,
    TYPE_UNION,
    TYPE_ARRAY,
    TYPE_POINTER,
    CUSTOM_INTEGER,
    CUSTOM_UINTEGER,
    CUSTOM_FLOAT,
    VAR_LOCAL,
    VAR_GLOBAL,
    VAR_PARAM,
    VAR_STATIC,
    CONST_LITERAL,
    FUNC_EXTERN,
    FUNC_INLINE
};

struct Symbol {
public:
    SymKind kind;
    SymVariant variant;
    int bytes = 0;
    int scopeLevel = 0;

    LLVMRadicalType radical;

    std::string name;
    bool isInitialized = false;
    bool isConst = false;
    bool isExported = false;
    bool isUsed = false;

    // Para funções
    std::vector<Symbol> parameters;
    std::shared_ptr<Symbol> returnType;

    // Para tipos estruturados
    std::map<std::string, Symbol> members;

    // Valor literal para constantes
    std::variant<std::monostate, long long int, double, std::string> literalValue;

    // Informações de localização no código
    int lineNumber = 0;
    int columnNumber = 0;
    std::string filename;

    Symbol(std::string name, SymKind kind, SymVariant variant, LLVMRadicalType radical, int bytes, int scopeLevel = 0)
        : name(name), kind(kind), variant(variant), radical(radical), bytes(bytes), scopeLevel(scopeLevel) {};

    Symbol(SymKind kind, SymVariant variant, LLVMRadicalType radical, int bytes, int scopeLevel = 0)
        : kind(kind), variant(variant), radical(radical), bytes(bytes), scopeLevel(scopeLevel) {};

    Symbol(SymKind kind, Symbol type) {
        this->kind = kind;
        this->variant = type.variant;
        this->radical = type.radical;
        this->bytes = type.bytes;
        this->scopeLevel = type.scopeLevel;
    };

    // Adicionar parâmetro a uma função
    void addParameter(const Symbol& param) {
        if (kind == FUNCTION) {
            parameters.push_back(param);
        }
    }

    // Definir tipo de retorno para função
    void setReturnType(const Symbol& type) {
        if (kind == FUNCTION) {
            returnType = std::make_shared<Symbol>(type);
        }
    }

    // Adicionar membro a um tipo estruturado
    void addMember(const std::string& memberName, const Symbol& memberSymbol) {
        if (kind == TYPE && (variant == TYPE_STRUCT || variant == TYPE_UNION)) {
            members[memberName] = memberSymbol;
        }
    }

    // Definir valor literal para constante
    template<typename T>
    void setLiteralValue(const T& value) {
        if (kind == CONSTANT || isConst) {
            literalValue = value;
            isInitialized = true;
        }
    }

    Symbol() = default;
};

struct Symbols {
private:
    int currentScope = 0;
    std::map<std::string, Symbol> symbols;
    std::vector<std::string> moduleStack; // Para suporte a módulos aninhados

public:
    Symbols() {
        int sys = sizeof(int);
        LLVMRadicalType sysl = (LLVMRadicalType) sys;

        // Tipos primitivos
        symbols.insert({"void",   Symbol("void", TYPE, TYPE_PRIMITIVE,  void_t, 0,    0)});

        // Inteiros
        symbols.insert({"int",    Symbol("int",  TYPE, TYPE_PRIMITIVE,  sysl,   sys,  0)});
        symbols.insert({"uint",   Symbol("uint", TYPE, TYPE_PRIMITIVE,  sysl,   sys,  0)});

        symbols.insert({"int8",   Symbol("int8",   TYPE, TYPE_PRIMITIVE,  i8,     1,    0)});
        symbols.insert({"int16",  Symbol("int16",  TYPE, TYPE_PRIMITIVE,  i16,    2,    0)});
        symbols.insert({"int32",  Symbol("int32",  TYPE, TYPE_PRIMITIVE,  i32,    4,    0)});
        symbols.insert({"int64",  Symbol("int64",  TYPE, TYPE_PRIMITIVE,  i64,    8,    0)});
        symbols.insert({"int128", Symbol("int128", TYPE, TYPE_PRIMITIVE,  i128,   16,   0)});
        symbols.insert({"int256", Symbol("int256", TYPE, TYPE_PRIMITIVE,  i256,   32,   0)});

        symbols.insert({"uint8",   Symbol("uint8",   TYPE, TYPE_PRIMITIVE, i8,     1,    0)});
        symbols.insert({"uint16",  Symbol("uint16",  TYPE, TYPE_PRIMITIVE, i16,    2,    0)});
        symbols.insert({"uint32",  Symbol("uint32",  TYPE, TYPE_PRIMITIVE, i32,    4,    0)});
        symbols.insert({"uint64",  Symbol("uint64",  TYPE, TYPE_PRIMITIVE, i64,    8,    0)});
        symbols.insert({"uint128", Symbol("uint128", TYPE, TYPE_PRIMITIVE, i128,   16,   0)});
        symbols.insert({"uint256", Symbol("uint256", TYPE, TYPE_PRIMITIVE, i256,   32,   0)});

        // Adicionar tipos de ponto flutuante
        symbols.insert({"float32", Symbol("float32", TYPE, TYPE_PRIMITIVE, i32,    4,    0)});
        symbols.insert({"float64", Symbol("float64", TYPE, TYPE_PRIMITIVE, i64,    8,    0)});

        // Adicionar tipos com nomes curtos
        symbols.insert({"i8",   Symbol("i8",   TYPE, TYPE_PRIMITIVE,  i8,     1,    0)});
        symbols.insert({"i16",  Symbol("i16",  TYPE, TYPE_PRIMITIVE,  i16,    2,    0)});
        symbols.insert({"i32",  Symbol("i32",  TYPE, TYPE_PRIMITIVE,  i32,    4,    0)});
        symbols.insert({"i64",  Symbol("i64",  TYPE, TYPE_PRIMITIVE,  i64,    8,    0)});
        symbols.insert({"i128", Symbol("i128", TYPE, TYPE_PRIMITIVE,  i128,   16,   0)});
        symbols.insert({"i256", Symbol("i256", TYPE, TYPE_PRIMITIVE,  i256,   32,   0)});

        symbols.insert({"u8",   Symbol("u8",   TYPE, TYPE_PRIMITIVE, i8,     1,    0)});
        symbols.insert({"u16",  Symbol("u16",  TYPE, TYPE_PRIMITIVE, i16,    2,    0)});
        symbols.insert({"u32",  Symbol("u32",  TYPE, TYPE_PRIMITIVE, i32,    4,    0)});
        symbols.insert({"u64",  Symbol("u64",  TYPE, TYPE_PRIMITIVE, i64,    8,    0)});
        symbols.insert({"u128", Symbol("u128", TYPE, TYPE_PRIMITIVE, i128,   16,   0)});
        symbols.insert({"u256", Symbol("u256", TYPE, TYPE_PRIMITIVE, i256,   32,   0)});

        // Adicionar tipos de ponto flutuante com nomes curtos
        symbols.insert({"f32", Symbol("f32", TYPE, TYPE_PRIMITIVE, i32,    4,    0)});
        symbols.insert({"f64", Symbol("f64", TYPE, TYPE_PRIMITIVE, i64,    8,    0)});

        // Adicionar tipo string
        symbols.insert({"string", Symbol("string", TYPE, TYPE_PRIMITIVE, i8, 0, 0)});

        // Adicionar tipo bool
        symbols.insert({"bool", Symbol("bool", TYPE, TYPE_PRIMITIVE, i1, 1, 0)});
    }

    // Entra em um novo escopo
    void enterScope() {
        currentScope++;
    }

    // Sai do escopo atual e remove símbolos desse escopo
    void exitScope() {
        if (currentScope > 0) {
            remove([](const Symbol& sym, int currentScope) {
                return sym.scopeLevel == currentScope;
            });
            currentScope--;
        }
    }

    // Obtém o escopo atual
    int getCurrentScope() const {
        return currentScope;
    }

    Symbol get(const std::string& id) {
        auto it = symbols.find(id);
        if (it == symbols.end()) {
            CompilerOutputs::Fatal("Símbolo não encontrado!");
        }
        return it->second;
    }

    // Verifica se símbolo existe
    bool contains(const std::string& id) const {
        return symbols.find(id) != symbols.end();
    }

    // Adiciona símbolo no escopo atual - CORRIGIDO: usa insert ou emplace
    void add(const std::string& id, const Symbol& symbol) {
        Symbol symWithScope = symbol;
        symWithScope.scopeLevel = currentScope;

        auto result = symbols.insert({id, symWithScope});
        if (!result.second) {
            result.first->second = symWithScope;
        }
    }

    // Adiciona símbolo com escopo específico - CORRIGIDO
    void add(const std::string& id, SymKind kind, SymVariant variant, LLVMRadicalType radical, int bytes) {
        Symbol newSymbol(id, kind, variant, radical, bytes, currentScope);
        auto result = symbols.insert({id, newSymbol});
        if (!result.second) {
            result.first->second = newSymbol;
        }
    }

    // Remove símbolos baseado em um filtro de escopo
    void remove(std::function<bool(const Symbol&, int)> filter) {
        std::vector<std::string> toRemove;

        // Primeiro identifica quais símbolos remover
        for (const auto& pair : symbols) {
            if (filter(pair.second, currentScope)) {
                toRemove.push_back(pair.first);
            }
        }

        // Depois remove os identificados
        for (const auto& key : toRemove) {
            symbols.erase(key);
        }
    }

    // Remove todos os símbolos de um escopo específico
    void removeByScope(int scopeLevel) {
        remove([scopeLevel](const Symbol& sym, int) {
            return sym.scopeLevel == scopeLevel;
        });
    }

    // Remove símbolos do escopo atual
    void removeCurrentScope() {
        removeByScope(currentScope);
    }

    // Remove símbolos com escopo maior que o atual (útil para cleanup)
    void removeScopesGreaterThanCurrent() {
        remove([](const Symbol& sym, int currentScope) {
            return sym.scopeLevel > currentScope;
        });
    }

    // Entrar em um módulo
    void enterModule(const std::string& moduleName) {
        moduleStack.push_back(moduleName);
    }

    // Sair do módulo atual
    void exitModule() {
        if (!moduleStack.empty()) {
            moduleStack.pop_back();
        }
    }

    // Obter o caminho completo do módulo atual
    std::string getCurrentModulePath() const {
        std::string path;
        for (const auto& module : moduleStack) {
            if (!path.empty()) path += "::";
            path += module;
        }
        return path;
    }

    // Adicionar símbolo no módulo atual
    void addToModule(const std::string& id, const Symbol& symbol) {
        std::string modulePath = getCurrentModulePath();
        std::string fullId = modulePath.empty() ? id : modulePath + "::" + id;

        Symbol symWithScope = symbol;
        symWithScope.scopeLevel = currentScope;

        auto result = symbols.insert({fullId, symWithScope});
        if (!result.second) {
            result.first->second = symWithScope;
        }
    }

    // Verificar se uma variável já foi inicializada
    bool isInitialized(const std::string& id) const {
        auto it = symbols.find(id);
        if (it != symbols.end()) {
            return it->second.isInitialized;
        }
        return false;
    }

    // Marcar variável como inicializada
    void setInitialized(const std::string& id) {
        auto it = symbols.find(id);
        if (it != symbols.end()) {
            it->second.isInitialized = true;
        }
    }

    // Marcar símbolo como usado
    void markAsUsed(const std::string& id) {
        auto it = symbols.find(id);
        if (it != symbols.end()) {
            it->second.isUsed = true;
        }
    }

    // Verificar símbolos não utilizados (útil para warnings)
    std::vector<std::string> getUnusedSymbols() const {
        std::vector<std::string> unused;
        for (const auto& pair : symbols) {
            if (!pair.second.isUsed && pair.second.scopeLevel > 0) {
                unused.push_back(pair.first);
            }
        }
        return unused;
    }

    // Lista todos os símbolos (útil para debugging)
    void listAll() const {
        for (const auto& pair : symbols) {
            std::cout << pair.first << " -> escopo: " << pair.second.scopeLevel
                      << ", tipo: " << pair.second.kind
                      << ", variant: " << pair.second.variant
                      << ", bytes: " << pair.second.bytes
                      << ", initialized: " << (pair.second.isInitialized ? "yes" : "no")
                      << ", used: " << (pair.second.isUsed ? "yes" : "no") << std::endl;
        }
    }

    // Limpa todos os símbolos (exceto os globais)
    void clearNonGlobal() {
        remove([](const Symbol& sym, int) {
            return sym.scopeLevel > 0;
        });
    }

    // Obter todos os símbolos de um determinado tipo
    std::vector<Symbol> getSymbolsByKind(SymKind kind) const {
        std::vector<Symbol> result;
        for (const auto& pair : symbols) {
            if (pair.second.kind == kind) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
};
