#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <cstdio>
#include <iostream>
#include <vector>

bool startMacro(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    *index += 1;
    result->~pNode();
    new(result) pNode(Macro::start());
    return true;
}
