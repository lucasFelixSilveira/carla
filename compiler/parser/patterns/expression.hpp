#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <iostream>
#include <vector>

#define LAST_KEYWORD TokenKind::END_KEYWORDS

bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx, bool internal=false) {
    auto first = ctx->at((*index)++);
    auto second = ctx->at((*index)++);

    auto x = first.kind == TokenCtxKind::Common, y = second.kind == TokenCtxKind::Common, z = std::get<Token>(second.content).kind == SEMICOLON;

    if( x && y ) {
        morgana::expr::root root = morgana::expr::single_expr { true, std::get<Token>(first.content).lexeme };
        auto expr = pExpression { "", root, std::vector<pContext>() };
        result->~pNode();
        new(result) pNode(expr);

        return true;
    }

    return false;
}
