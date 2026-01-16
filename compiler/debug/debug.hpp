// Isto auxilia na depuracao do sistema

#pragma once

#if CARLA_DEBUG

#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <variant>
#include <vector>
#include <string>
#include "../parser/ast.hpp"
#include "../tokenizer/token_kind.hpp"
#include "../compiler_outputs.hpp"


template<typename T>
inline
void
dvecprint(std::vector<T> vec)
{
    for( T value : vec ) std::cout << value.to_string() << std::endl;
}

void
printCtx(const pTokenCtx& ctx, std::string indent = "", bool isLast = true) {
    std::cout << indent;
    if (!indent.empty()) {
        std::cout << (isLast ? "└── " : "├── ");
    }

    if (ctx.kind == Block) {
        std::cout << "Block" << std::endl;

        const auto& children = std::get<std::vector<pTokenCtx>>(ctx.content);
        for (size_t i = 0; i < children.size(); ++i) {
            bool last = (i == children.size() - 1);
            printCtx(children[i], indent + (isLast ? "  " : "│   "), last);
        }
    } else if (ctx.kind == Common) {
        const Token& tk = std::get<Token>(ctx.content);
        std::cout << "Token: " << tokenKindToString(tk.kind);

        if (!tk.lexeme.empty()) {
            std::cout << " (\"" << tk.lexeme << "\")";
        }

        std::cout << std::endl;
    }
}

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

void printNode(pNode node, int indent);

void printNodes(const std::vector<pNode>& nodes, int indent = 0) {
    for( const auto node : nodes ) {
        printNode(node, indent);
    }
}

std::string printExpressionNodes(pExpressionNodes expr, int tabs, std::string keyword);

std::string printExpression(pExpression expr, int tabs = 0, std::string keyword = "") {
    return printExpressionNodes(expr.nodes, tabs, keyword);
}

void printNode(pNode node, int indent) {
    printIndent(indent);
    std::cout << "NodeKind: " << pKindStr(node.kind) << "\n";

    std::visit([&](auto&& arg) {
        int ident = 0;
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            printIndent(indent);
            std::cout << "(empty)\n";
        } else if constexpr (std::is_same_v<T, pDeclaration>) {
            printIndent(indent);
            std::cout << "Declaration: name = " << arg.name
                      << ", complement = " << pDeclarationStr(arg.complement) << "\n";
        } else if constexpr (std::is_same_v<T, pLambda>) {
            printIndent(indent);
            std::cout << "Lambda: pub = " << (arg.pub ? "true" : "false") << "\n";
            printIndent(indent);
            std::cout << "Args:\n";
        } else if constexpr (std::is_same_v<T, pExpression>) {
            std::cout << printExpression(std::get<pExpression>(node.values)) << "\n";
        }
    }, node.values);
}

std::string makeIndent(int indent) {
    std::stringstream ss;
    for( int i = 0; i > indent; --i ) ss << "  ";
    return ss.str();
}

std::string printExpressionNodes(pExpressionNodes expr, int tabs=10, std::string keyword="") {
    std::stringstream ss;

    bool lhs_literal = std::holds_alternative<std::string>(expr.lhs);
    bool rhs_literal = std::holds_alternative<std::string>(expr.rhs);

    bool lhs_nodes = std::holds_alternative<std::shared_ptr<pExpressionNodes>>(expr.lhs);
    bool rhs_nodes = std::holds_alternative<std::shared_ptr<pExpressionNodes>>(expr.rhs);

    bool lhs_expr = std::holds_alternative<std::shared_ptr<pExpression>>(expr.lhs);
    bool rhs_expr = std::holds_alternative<std::shared_ptr<pExpression>>(expr.rhs);

    std::string op = tokenKindToString(expr.op);

    if( lhs_literal ) {
        ss << std::get<std::string>(expr.lhs)
           << " " << op << " ";
    }

    if( lhs_nodes ) {
        ss <<  printExpressionNodes(*std::get<std::shared_ptr<pExpressionNodes>>(expr.lhs), tabs-1)
           << " " << op << " ";
    }

    if( lhs_expr ) {
        auto _expr = *std::get<std::shared_ptr<pExpression>>(expr.lhs);

        ss << printExpression(_expr, tabs-1)
           << " " << op << " ";
    }

    if( rhs_literal ) {
        ss << std::get<std::string>(expr.rhs);
    }

    if( rhs_nodes ) {
        ss << printExpressionNodes(*std::get<std::shared_ptr<pExpressionNodes>>(expr.rhs), tabs-1);
    }

    if( rhs_expr ) {
        auto _expr = *std::get<std::shared_ptr<pExpression>>(expr.rhs);
        ss << printExpression(_expr, tabs-1);
    }

    return ss.str();
}

#endif
