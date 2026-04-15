#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <vector>

bool returnStatement(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    pContext after = (*ctx)[*index + 1];
    (*index)++;

    if( after.kind == Common && std::get<Token>(after.content).kind == TokenKind::SEMICOLON ) {
        *result = pNode(RetStatement(true));
        return true;
    }

    *result = pNode(RetStatement(false));
    return true;
}
