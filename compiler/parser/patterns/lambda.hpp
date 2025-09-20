#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <vector>

template<typename T, typename X>
bool lambda(pNode *result, Symbols *sym, T index, X ctx) {
    pContext arguments = (*ctx)[*index];
    if(! std::holds_alternative<std::vector<pContext>>(arguments.content) ) return false;
    const auto& blockContent = std::get<std::vector<pContext>>(arguments.content);

    pContext body = (*ctx)[*index + 1];
    if( arguments.kind != Block || body.kind != Block ) return false;

    std::vector<pNode> args;
    size_t internal = 0;

    while(internal < blockContent.size()) {
        if( internal + 1 >= blockContent.size() ) return false;

        const pContext& typeCtx = blockContent[internal];
        if( typeCtx.kind != Common || !std::holds_alternative<Token>(typeCtx.content) ) return false;
        Token typeTk = std::get<Token>(typeCtx.content);
        if( ! sym->contains(typeTk.lexeme) || sym->get(typeTk.lexeme).kind != TYPE ) return false;

        const pContext& idCtx = blockContent[internal + 1];
        if( idCtx.kind != Common || !std::holds_alternative<Token>(idCtx.content) ) return false;
        Token idTk = std::get<Token>(idCtx.content);
        if( idTk.kind != IDENTIFIER || idTk.lexeme.empty() ) return false;

        args.push_back(pNode(dArgument, sym->get(typeTk.lexeme), idTk.lexeme));

        internal += 2;
        if( internal >= blockContent.size() ) break;

        const pContext& symbol = blockContent[internal];
        if( symbol.kind != Common || !std::holds_alternative<Token>(symbol.content) ) return false;
        if( std::get<Token>(symbol.content).kind != COMMA ) break;
        internal++;
    }

    *result = pNode(NODE_LAMBDA, args);
    *index += 2;
    return true;
}
