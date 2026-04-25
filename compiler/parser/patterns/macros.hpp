#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <iostream>
#include <vector>

bool startMacro(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    *index += 1;
    result->~pNode();
    new(result) pNode(Macro::start());
    return true;
}

bool externStatement(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    auto identifier = (*ctx)[++(*index)];
    if( identifier.kind != Common ) return false;

    auto token = std::get<Token>(identifier.content);
    std::cout << "\nextern: " << token.lexeme << std::endl;

    (*index)++;
    result->~pNode();
    new(result) pNode(Macro::extern_(token.lexeme));
    return true;
}
