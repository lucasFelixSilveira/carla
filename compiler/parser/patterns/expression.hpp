#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
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

std::pair<bool, std::string> try_comptime(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    std::string value;
    std::string lexame;
    auto first = (*ctx)[(*index)++];
    if( first.kind == TokenCtxKind::Common ) {
        auto content = std::get<Token>(first.content);
        if(! (is_number(content.lexeme) || is_string(content.lexeme)) ) return { false, std::string() };
        value = content.lexeme, lexame = content.lexeme;
    }

    std::vector<TokenKind> operators({ SEMICOLON, PLUS });

    auto second = (*ctx)[(*index)++];
    if( second.kind != TokenCtxKind::Common ) return { false, std::string() };

    auto token = std::get<Token>(second.content);
    if(! contains(operators, token.kind) ) return { false, std::string() };

    if( is_string(value) ) switch(token.kind) {
        case SEMICOLON: return { true, value };
        case PLUS: {
            auto [ success, rhs ] = try_comptime(result, sym, index, ctx);
            if( success && is_string(rhs) ) {
                return { true, '"' + value.substr(1, value.length()-2) + rhs.substr(1, rhs.length()-2) + '"' };
            }
        }
        default: return { false, std::string() };
    }

    return { false, std::string() };
}

bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    size_t old = *index;
    auto [ success, value ] = try_comptime(result, sym, index, ctx);

    if( success ) {
        morgana::expr::root root = morgana::expr::single_expr { success, value };
        auto expr = pExpression { "", root, std::vector<pContext>() };
        result->~pNode();
        new(result) pNode(expr);
        return true;
    }

    *index = old - 1;
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
                    std::vector<std::string> ssargs;
                    for( auto arg : args ) {
                        if( arg.kind != TokenCtxKind::Common ) return false;
                        auto token = std::get<Token>(arg.content);
                        if( token.lexeme == ")" ) break;
                        if( token.lexeme == "," ) continue;
                        ssargs.push_back(token.lexeme);
                    }

                    root = morgana::expr::call_expr { token.lexeme, ssargs };
                    auto expr = pExpression { "", root, std::vector<pContext>() };

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
