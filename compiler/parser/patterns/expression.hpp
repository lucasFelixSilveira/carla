#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#define LAST_KEYWORD TokenKind::END_KEYWORDS

bool contains(const std::array<std::string, 2>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

// int8 (b =  (x + 1))
// b = arg1
// arg1 = x + 1
// x + 1
// ^
bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx, bool internal=false) {
    pContext data = (*ctx)[*index];

    std::array<std::string, 2> exprKeywords = { "try", "if" };

    pExpression expr;

    if( data.kind == TokenCtxKind::Common ) {
        std::string lexame = std::get<Token>(data.content).lexeme;
        if( contains(exprKeywords, lexame) ) {
            expr.keyword = lexame;
            *index += 1;
        }
    }

    pExpressionNodes nodes;

    pContext op = (*ctx)[*index + 1];

    if( op.kind != TokenCtxKind::Common ) return false;
    Token opdata = std::get<Token>(op.content);
    int pos = opdata.kind;
    if(! (pos >= 7 && pos <= 15) ) return false;
    nodes.op = (TokenKind) pos;


    pContext stmt[2] = { (*ctx)[*index], (*ctx)[*index + 2] };
    *index += 3;

    for( int i = 0; i < 2; i++ ) {
        pContext side = stmt[i];

        if( side.kind == TokenCtxKind::Block ) {
            pNode result;
            long unsigned int j = 0;

            std::vector<pContext> values = std::get<std::vector<pContext>>(side.content);
            if(! expression(&result, sym, &j, &values, true) ) return false;

            pExpression data = std::get<pExpression>(result.values);
            if( i == 0 ) nodes.lhs = std::make_shared<pExpression>(data);
            else nodes.rhs = std::make_shared<pExpression>(data);

            continue;
        }

        if( side.kind == TokenCtxKind::Common ) {
            int pos = std::get<Token>(side.content).kind;
            if( pos >= 39 && pos <= LAST_KEYWORD ) return false;

            if( i == 0 ) nodes.lhs = std::get<Token>(side.content).lexeme;
            else nodes.rhs = std::get<Token>(side.content).lexeme;
        }
    }

    pContext check_next;
    if( *index >= ctx->size() && internal ) {
        expr.nodes = nodes;
        goto __final_expr_parser;
    }

    check_next = (*ctx)[*index];
    if( check_next.kind == TokenCtxKind::Common ) {
        Token tkData = std::get<Token>(check_next.content);
        int pos = tkData.kind;

        if( pos == TokenKind::SEMICOLON ) {
            expr.nodes = nodes;
            (*index)++;
            goto __final_expr_parser;
        }

        if(! (pos >= 7 && pos <= 15) ) return false;

        *index -= 1;
        unsigned long int old = *index;
        pNode result;
        if(! expression(&result, sym, index, ctx) ) return false;

        pExpressionNodes parent;
        parent.lhs = std::make_shared<pExpressionNodes>(nodes);
        parent.op  = tkData.kind;
        parent.rhs = std::make_shared<pExpression>(std::get<pExpression>(result.values));

        expr.nodes = parent;
    }

    __final_expr_parser: {
        *result = pNode(expr);
        return true;
    }
}
