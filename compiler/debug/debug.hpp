// Isto auxilia na depuracao do sistema

#pragma once

#include <iostream>
#include <ostream>
#include <variant>
#include <vector>
#include <string>
#include "../parser/ast.hpp"
#include "../tokenizer/token_kind.hpp"

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

void printNode(pNode node, int indent) {
    printIndent(indent);
    std::cout << "NodeKind: " << pKindStr(node.kind) << "\n";

    std::visit([indent](auto&& arg) {
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
            // printNodes(arg.args, indent + 1);
        }
    }, node.values);
}
