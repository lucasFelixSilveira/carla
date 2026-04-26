#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <cstdio>
#include <regex>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#define LAST_KEYWORD TokenKind::END_KEYWORDS

template <typename T>
bool contains(const std::vector<T> vec, T val) {
    for( T x : vec )
    /* -> */ if( x == val ) return true;
    return false;
}

inline bool is_string(const std::string str)
{ return std::regex_match(str, std::regex("\\\".*\\\"")); };

inline bool is_number(const std::string str)
{ return std::regex_match(str, std::regex("[0-9]+(\\.[0.9]+)?")); };

inline bool is_identifier(const std::string str)
{ return std::regex_match(str, std::regex("[A-Za-z_][A-Za-z0-9_]*")); };

bool expression(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx);

std::tuple<bool, std::string, morgana::expr::expr_type> try_comptime(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    std::string value;
    std::string lexame;
    auto first = (*ctx)[(*index)++];
    if( first.kind == TokenCtxKind::Common ) {
        auto content = std::get<Token>(first.content);
        if(! (is_number(content.lexeme) || is_string(content.lexeme)) ) return { false, std::string(), morgana::expr::nil };
        value = content.lexeme, lexame = content.lexeme;
    }

    std::vector<TokenKind> operators({ SEMICOLON, COMMA, PLUS });

    if( (*index) + 1 > ctx->size() ) return {
        true,
        value,
        is_string(value) ? morgana::expr::string : morgana::expr::nil
    };

    auto second = (*ctx)[(*index)++];
    if( second.kind != TokenCtxKind::Common ) return { false, std::string(), morgana::expr::nil };

    auto token = std::get<Token>(second.content);
    if(! contains(operators, token.kind) ) return { false, std::string(), morgana::expr::nil };

    if( is_string(value) ) switch(token.kind) {
        case SEMICOLON:
        case COMMA: return { true, value, morgana::expr::string };
        case PLUS: {
            auto [ success, rhs, type ] = try_comptime(result, sym, index, ctx);
            if( success && is_string(rhs) ) {
                return { true, '"' + value.substr(1, value.length()-2) + rhs.substr(1, rhs.length()-2) + '"', type };
            }
        }
        default: return { false, std::string(), morgana::expr::nil };
    }

    return { false, std::string(), morgana::expr::nil };
}


unsigned long long times = 0, pos = 0;
std::tuple<bool, std::tuple<std::vector<pNode>, std::vector<std::string>>> arguments(Symt *sym, std::vector<pContext> args) {
    size_t index = 0;

    std::vector<pNode> fake_nodes;
    std::vector<std::string> fake_identifiers;

    while(index < args.size()) {
        pNode result;

        if( expression(&result, sym, &index, &args) ) {
            auto data = std::get<pExpression>(result.values);
            morgana::expr::root root = data.root;

            std::string identifier = "fake_" + std::to_string(times) + "_" + std::to_string(pos++) + "";
            Symbol *s;

            if( std::holds_alternative<morgana::expr::single_expr>(root) ) {
                auto single = std::get<morgana::expr::single_expr>(root);

                if( single.type == morgana::expr::string ) s = sym->findSymbol("string");
                if( single.type == morgana::expr::numeric ) s = sym->findSymbol("usize");
            }

            fake_identifiers.push_back(identifier);

            std::vector<pContext> specialCtx;
            fake_nodes.push_back(pNode(dHopefull, *s, identifier, specialCtx));
            fake_nodes.push_back(data);
            continue;
        }

        return { false, { {}, {} } };
    }

    return { true, { fake_nodes, fake_identifiers } };
}

bool expression(pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx) {
    size_t old = *index;
    auto [ success, value, type ] = try_comptime(result, sym, index, ctx);

    if( success ) {
        morgana::expr::root root = morgana::expr::single_expr { success, value, type };
        auto expr = pExpression { "", root, {}, std::monostate() };
        result->~pNode();
        new(result) pNode(expr);
        return true;
    }

    *index = old;
    auto first = (*ctx)[(*index)++];
    if( first.kind != TokenCtxKind::Common ) return false;

    auto token = std::get<Token>(first.content);
    if( is_identifier(token.lexeme) ) {
        auto symbol = sym->findSymbol(token.lexeme);
        if(! std::holds_alternative<morgana::ffi_callable>(*symbol) ) return false;

        auto var = std::get<morgana::ffi_callable>(*symbol);
        morgana::expr::root root;

        bool lhs_call = false;
        std::vector<pTokenCtx> args;
        auto next = (*ctx)[(*index)++];
        if( next.kind == TokenCtxKind::Block ) {
            args = std::get<std::vector<pTokenCtx>>(next.content);
            lhs_call = true;
            next = (*ctx)[(*index)++];
        }

        if( next.kind != TokenCtxKind::Common ) return false;
        auto operator_tk = std::get<Token>(next.content);
        switch(operator_tk.kind) {
            case SEMICOLON: {
                if( lhs_call ) {
                    auto [ success, data ] = arguments(sym, args);
                    if(! success) return false;

                    auto [ fake_nodes, fake_identifiers ] = data;
                    root = morgana::expr::call_expr { token.lexeme, fake_identifiers };
                    auto expr = pExpression("", root, std::vector<pContext>(), fake_nodes);

                    result->~pNode();
                    new(result) pNode(expr);
                    return true;
                }
            } break;
            default: return false;
        }

        return false;
    }

    return false;
}
