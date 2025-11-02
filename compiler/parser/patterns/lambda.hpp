#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <memory>
#include <variant>
#include <vector>

bool lambda(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    pContext arguments = (*ctx)[*index];
    if(! std::holds_alternative<std::vector<pContext>>(arguments.content) ) return false;
    const auto& blockContent = std::get<std::vector<pContext>>(arguments.content);

    pContext body = (*ctx)[*index + 1];
    if( arguments.kind != Block || body.kind != Block ) return false;

    morgana::function::args argst;
    morgana::desconstruct::values argsn;
    size_t internal = 0;

    while(internal < blockContent.size()) {
        if( internal + 1 >= blockContent.size() ) return false;

        Symbol* type;
        const pContext& typeCtx = blockContent[internal];
        if( typeCtx.kind != Common || !std::holds_alternative<Token>(typeCtx.content) ) return false;
        Token typeTk = std::get<Token>(typeCtx.content);
        if( (type = sym->findSymbol(typeTk.lexeme)) == nullptr || !std::holds_alternative<std::shared_ptr<morgana::type>>(*type) ) return false;

        const pContext& idCtx = blockContent[internal + 1];
        if( idCtx.kind != Common || !std::holds_alternative<Token>(idCtx.content) ) return false;
        Token idTk = std::get<Token>(idCtx.content);
        if( idTk.kind != IDENTIFIER || idTk.lexeme.empty() ) return false;

        argst.push_back(std::get<std::shared_ptr<morgana::type>>(*type));
        argsn.push_back(idTk.lexeme);

        internal += 2;
        if( internal >= blockContent.size() ) break;

        const pContext& symbol = blockContent[internal];
        if( symbol.kind != Common || !std::holds_alternative<Token>(symbol.content) ) return false;
        if( std::get<Token>(symbol.content).kind != COMMA ) return false;
        internal++;
    }

    *result = pNode(NODE_LAMBDA, argst, argsn, body);
    *index += 2;
    return true;
}
