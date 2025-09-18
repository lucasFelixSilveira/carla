// Faz o Handler dos possiveis Patterns

#pragma once

#include "../utils/result.hpp"
#include "ast.hpp"

template<typename T, typename X>
Result pattern(pNode *result, T index, X ctx) {
    const pContext& context = (*ctx)[index];
    Token tk = std::get<Token>(context.content);

    switch(tk.kind) {
    case IDENTIFIER: {
        if( declaration(result, index, ctx) ) return Some{};
    } break;
    default: return Err{"Uknown pattern"};
    }
}
