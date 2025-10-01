#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"

bool declaration(pNode *result, Symbols *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    Token tk = std::get<Token>((*ctx)[*index].content);
    Symbol type = Symbol();

    // Temos certeza que o valor é um identificador por causa da checagem
    // anterior feita pelo 'pattern.hpp'
    if(! sym->contains(tk.lexeme) ) return false;
    if( (type = sym->get(tk.lexeme)).kind != TYPE ) return false;
    /// TODO: checagem de ponteiros
    /// TODO: checagem de arrays

    pContext identifier = (*ctx)[*index + 1];
    if( identifier.kind != Common ) return false;
    Token identifierToken = std::get<Token>(identifier.content);
    if( identifierToken.kind != IDENTIFIER ) return false;
    if( identifierToken.lexeme.empty() ) return false;

    // Checagem do tipo da declaração
    pContext symbol = (*ctx)[*index + 2];
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

        *result = pNode(declarationType, type, identifierToken.lexeme);
        *index += 2;
        return true;
    };

    default: return false;
    }
}
