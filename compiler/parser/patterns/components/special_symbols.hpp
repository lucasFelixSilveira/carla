#pragma once

#include "../../symbols.hpp"
#include "../../ast.hpp"
#include <cstdlib>
#include <memory>
#include <variant>
#include <vector>


bool parse_components_special(Symbol* symbol, Symt *sym, long unsigned int current, long unsigned int *index, const std::vector<pContext>* ctx) {
    auto s = std::get<std::shared_ptr<special>>(*symbol);

    // Keywords não são tipos.
    if( s->kind == special::Keyword ) return false;

    pContext block = (*ctx)[current + 1];
    if( block.kind != Block ) return false;
    const auto& blockContent = std::get<std::vector<pContext>>(block.content);
    size_t internal = 0;
    int param = 0;

    while(internal < blockContent.size()) {
        if( internal + 1 >= blockContent.size() ) break;

        Symbol* type;
        const pContext& idCtx = blockContent[internal];
        if( idCtx.kind != Common || !std::holds_alternative<Token>(idCtx.content) ) return false;
        Token idTk = std::get<Token>(idCtx.content);
        if( idTk.kind != IDENTIFIER && idTk.kind == NUMBER ) return false;

        if( idTk.kind == IDENTIFIER && sym->findSymbol(idTk.lexeme) == nullptr ) return false;
        if( idTk.kind == NUMBER ) {
            Symbol* res;
            if( (res = sym->findSymbol(s->types.at(param))) == nullptr ) return false;

            if( std::holds_alternative<std::shared_ptr<morgana::type>>(*res) ) {
                auto type = std::get<std::shared_ptr<morgana::type>>(*res);
                if( type->get_radical() != morgana::radical::Integer ) return false;
            }
        }

        internal++;
        if( internal >= blockContent.size() ) break;

        const pContext& symbol = blockContent[internal];
        if( symbol.kind != Common || !std::holds_alternative<Token>(symbol.content) ) return false;
        if( std::get<Token>(symbol.content).kind != COMMA ) return false;
        internal++;
    }

    return true;
}

std::shared_ptr<morgana::type> assemble_special_symbol(Symt* sym, Symbol *symbol, std::vector<pContext> ctx) {
    auto s = std::get<std::shared_ptr<special>>(*symbol);

    // stackptr(int64, 32) -> [32:i64]
    if( s->name == "stackptr" ) {
        pContext type = ctx[0];
        pContext size = ctx[2];

        Symbol* s = sym->findSymbol(std::get<Token>(type.content).lexeme);
        std::shared_ptr<morgana::type> into = std::get<std::shared_ptr<morgana::type>>(*s);
        auto copy = *into;
        int sizei = std::atoi(std::get<Token>(size.content).lexeme.c_str());
        copy.vec(sizei);

        return std::make_shared<morgana::type>(copy);
    }
}
