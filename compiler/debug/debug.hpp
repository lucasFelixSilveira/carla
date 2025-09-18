// Isto auxilia na depuracao do sistema

#pragma once

#include <iostream>
#include <ostream>
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
