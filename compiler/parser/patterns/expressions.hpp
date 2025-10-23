#pragma once
#include "../ast.hpp"
#include "../symbols.hpp"
#include <stack>
#include <memory>
#include <variant>
#include <string>
#include <vector>

static inline int precedence(const std::string& op) {
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

static inline std::string getOperator(TokenKind kind) {
    switch (kind) {
        case TokenKind::PLUS: return "+";
        case TokenKind::MINUS: return "-";
        case TokenKind::STAR: return "*";
        case TokenKind::SLASH: return "/";
        default: return "";
    }
}

static std::shared_ptr<pNode> makeExprNode(
    std::variant<double, std::string, std::shared_ptr<pNode>> left,
    const std::string& op,
    std::variant<double, std::string, std::shared_ptr<pNode>> right
) {
    return std::make_shared<pNode>(pExpression{left, op, right});
}

bool expressions(pNode *result, Symbols *sym, size_t *index, const std::vector<pContext> *ctx) {
    std::vector<std::variant<double, std::string, std::shared_ptr<pNode>>> values;
    std::vector<std::string> ops;

    size_t i = *index;

    auto applyOp = [&]() {
        if (values.size() < 2 || ops.empty()) return false;
        auto right = values.back(); values.pop_back();
        auto left = values.back(); values.pop_back();
        std::string op = ops.back(); ops.pop_back();
        auto node = makeExprNode(left, op, right);
        values.push_back(node);
        return true;
    };

    while (i < ctx->size()) {
        const pContext &c = (*ctx)[i];
        if (c.kind != Common && c.kind != Block) break;

        if (c.kind == Common && std::holds_alternative<Token>(c.content)) {
            Token tk = std::get<Token>(c.content);

            if (tk.kind == NUMBER) {
                values.push_back(std::stod(tk.lexeme));
            } else if (tk.kind == IDENTIFIER) {
                values.push_back(tk.lexeme);
            } else if (tk.kind == STRING) {
                // Adiciona a string literal (sem as aspas)
                std::string str_literal = tk.lexeme;
                // Remove as aspas do início e fim
                if (str_literal.size() >= 2 &&
                    ((str_literal.front() == '"' && str_literal.back() == '"') ||
                     (str_literal.front() == '\'' && str_literal.back() == '\''))) {
                    str_literal = str_literal.substr(1, str_literal.size() - 2);
                }
                values.push_back(str_literal);
            } else if (tk.kind == PLUS || tk.kind == MINUS || tk.kind == STAR || tk.kind == SLASH) {
                std::string op = getOperator(tk.kind);
                while (!ops.empty() && precedence(ops.back()) >= precedence(op)) {
                    if (!applyOp()) return false;
                }
                ops.push_back(op);
            } else if (tk.kind == SEMICOLON) {
                i++;
                break;
            }
        } else if (c.kind == Block && std::holds_alternative<std::vector<pContext>>(c.content)) {
            // Tratar o bloco como uma subexpressão (parênteses em Carla)
            const auto& block_ctx = std::get<std::vector<pContext>>(c.content);
            size_t block_index = 0;
            pNode sub_result;
            // Chama recursivamente a função expressions para processar o conteúdo do bloco
            if (!expressions(&sub_result, sym, &block_index, &block_ctx)) {
                return false;
            }
            // Verifica se o bloco foi totalmente consumido
            if (block_index != block_ctx.size()) {
                return false;
            }
            // Adiciona o resultado da subexpressão como um nó
            values.push_back(std::make_shared<pNode>(sub_result));
        }

        i++;
    }

    // Aplica operadores restantes
    while (!ops.empty()) {
        if (!applyOp()) return false;
    }

    // Verifica se sobrou exatamente um valor
    if (values.size() != 1) return false;

    // Atribui o resultado
    if (std::holds_alternative<std::shared_ptr<pNode>>(values.back())) {
        *result = *std::get<std::shared_ptr<pNode>>(values.back());
    } else {
        auto simple = pExpression{values.back(), "", 0.0};
        *result = pNode(simple);
    }

    *index = i;
    return true;
}
