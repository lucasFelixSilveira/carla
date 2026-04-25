#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <vector>

bool returnStatement(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    pContext after = (*ctx)[*index + 1];
    (*index)++;

    if( after.kind == Common && std::get<Token>(after.content).kind == TokenKind::SEMICOLON ) {
        result->~pNode();
        new(result) pNode(NODE_RETURN, SimpleStatement(true));
        return true;
    }

    result->~pNode();
    new(result) pNode(NODE_RETURN, SimpleStatement(false));
    return true;
}

bool putsStatement(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    pContext after = (*ctx)[*index + 1];
    (*index)++;

    result->~pNode();
    new(result) pNode(NODE_PUTS, SimpleStatement(true));
    return true;
}
