#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include "components/special_symbols.hpp"

#include <memory>
#include <string>
#include <variant>
#include <vector>


// Declaração comum, onde já se sabe o tipo futuro ou o tipo do valor/resultado.
bool declaration(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    Token tk = std::get<Token>((*ctx)[*index].content);
    Symbol* type;

    // Temos certeza que o valor é um identificador por causa da checagem
    // anterior feita pelo 'pattern.hpp'
    if( (type = sym->findSymbol(tk.lexeme)) == nullptr ) return false;
    if(! (std::holds_alternative<std::shared_ptr<morgana::type>>(*type) || std::holds_alternative<std::shared_ptr<special>>(*type)) ) return false;

    int jumped = 1;
    std::vector<pContext> specialCtx = {};
    if( std::holds_alternative<std::shared_ptr<special>>(*type) ) {
        if(! parse_components_special(type, sym, *index, index, ctx) ) return false;
        specialCtx = std::get<std::vector<pContext>>((*ctx)[*index + 1].content);
        jumped += 1;
    }

    pContext ptr = (*ctx)[*index + jumped];
    if( ptr.kind != Common ) return false;

    while(true) {
        Token ptrTk = std::get<Token>(ptr.content);
        if( ptrTk.kind != STAR ) break;

        ptr = (*ctx)[*index + (++jumped)];
        if( ptr.kind != Common ) return false;
    }

    int ptrSize = (jumped - 1);

    pContext identifier = ptr;
    if( ptrSize > 0 ) identifier = (*ctx)[*index + jumped];

    Token identifierToken = std::get<Token>(identifier.content);
    if( identifierToken.kind != IDENTIFIER ) return false;
    if( identifierToken.lexeme.empty() ) return false;


    // Checagem do tipo da declaração
    pContext symbol = (*ctx)[*index + 1 + jumped];
    if( symbol.kind != Common ) return false;
    Token symbolToken = std::get<Token>(symbol.content);
    switch(symbolToken.kind) {
    case COLON:
    case SEMICOLON:
    case EQUAL: {
        pDeclaration_e declarationType =
            (symbolToken.kind == COLON) ? dIterable :
            (symbolToken.kind == SEMICOLON) ? dHopeless :
            dHopefull;

        // Adicionar variável à tabela de símbolos durante o parsing
        std::shared_ptr<morgana::type> into;
        if( std::holds_alternative<std::shared_ptr<morgana::type>>(*type) ) into = std::get<std::shared_ptr<morgana::type>>(*type);
        else if( std::holds_alternative<std::shared_ptr<special>>(*type) ) into = assemble_special_symbol(sym, type, specialCtx);

        morgana::variable v(identifierToken.lexeme, into, true);
        sym->addSymbol(identifierToken.lexeme, v.shared());

        *result = pNode(declarationType, *type, identifierToken.lexeme, specialCtx);
        *index += 2 + ptrSize;
        return true;
    };

    default: return false;
    }
}
