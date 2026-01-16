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
//  (1 + 2) + ((2+(3-4))+ )
// { 1 PLUS 2 } PLUS {   {     2 PLUS  { 3 MINUS 4     }   } PLUS {   } }
// 1 + 2 + (3 - 4) + 5
// (1+2) + ((3-4) + 5)
// lhs = 1 + 2
//     - lhs 1
//     - rhs 2
// rhs ((3-4) + 5)
//     - lhs (3-4)
//         -lhs 3
//         -rhs 4
//     -rhs 5

std::vector<pExpression> exprs;

// bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx, bool internal=false) {
//     pContext data = (*ctx)[*index];

//     std::array<std::string, 2> exprKeywords = { "try", "if" };

//     pExpression expr;

//     if( data.kind == TokenCtxKind::Common ) {
//         std::string lexame = std::get<Token>(data.content).lexeme;
//         if( contains(exprKeywords, lexame) ) {
//             expr.keyword = lexame;
//             *index += 1;
//         }
//     }

//     pExpressionNodes nodes;

//     pContext op = (*ctx)[*index + 1];
//     if( op.kind != TokenCtxKind::Common ) return false;

//     Token opdata = std::get<Token>(op.content);
//     int pos = opdata.kind;
//     if(! (pos >= 7 && pos <= 15) ) return false;

//     nodes.op = (TokenKind) pos;

//     pContext stmt[2] = { (*ctx)[*index], (opdata.kind == TokenKind::SEMICOLON) ? pContext() : (*ctx)[*index + 2] };

//     int exprlen = ((opdata.kind == TokenKind::SEMICOLON) ? 1 : 2);
//     *index += ((opdata.kind == TokenKind::SEMICOLON) ? 1 : 3);

//     for( int i = 0; i < exprlen; i++ ) {
//         pContext side = stmt[i];

//         if( side.kind == TokenCtxKind::Block ) {
//             pNode result;
//             long unsigned int j = 0;

//             std::vector<pContext> values = std::get<std::vector<pContext>>(side.content);
//             if(! expression(&result, sym, &j, &values, true) ) return false;

//             pExpression data = std::get<pExpression>(result.values);
//             if( i == 0 ) nodes.lhs = std::make_shared<pExpression>(data);
//             else nodes.rhs = std::make_shared<pExpression>(data);

//             continue;
//         }

//         if( side.kind == TokenCtxKind::Common ) {
//             int pos = std::get<Token>(side.content).kind;
//             if( pos >= 39 && pos <= LAST_KEYWORD ) return false;

//             if( i == 0 ) nodes.lhs = std::get<Token>(side.content).lexeme;
//             else nodes.rhs = std::get<Token>(side.content).lexeme;
//         }
//     }

//     if( exprlen == 1 ) {
//         nodes.op = TokenKind::PLUS;
//         nodes.rhs = "0";
//     }

//     if( internal && *index >= ctx->size() ) {
//         expr.nodes = nodes;
//         *result = pNode(expr);
//         return true;
//     }

//     pContext check_next = (*ctx)[*index];
//     if( check_next.kind == TokenCtxKind::Common ) {
//         Token tk = std::get<Token>(check_next.content);

//         if( tk.kind == TokenKind::SEMICOLON ) {
//             expr.nodes = nodes;
//             (*index)++;
//             *result = pNode(expr);
//             return true;
//         }

//         int pos = tk.kind;
//         if(! (pos >= 7 && pos <= 15) ) return false;

//         *index += 1;
//         unsigned long int old = *index;
//         pNode result;
//         if(! expression(&result, sym, index, ctx) ) return false;

//         pExpressionNodes parent;
//         parent.lhs = std::make_shared<pExpressionNodes>(nodes);
//         parent.op  = tk.kind;
//         parent.rhs = std::make_shared<pExpression>(std::get<pExpression>(result.values));

//         expr.nodes = parent;
//     }

//     *result = pNode(expr);
//     return true;
// }


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

    morgana::expr::nodes nodes;

    pContext op = (*ctx)[*index + 1];
    if( op.kind != TokenCtxKind::Common ) return false;

    Token opdata = std::get<Token>(op.content);
    int pos = opdata.kind;
    if(! (pos >= 7 && pos <= 15) ) return false;

    auto operand = [&](TokenKind kind) -> int {
        if( kind == TokenKind::PLUS )  return morgana::expr::add;
        if( kind == TokenKind::MINUS ) return morgana::expr::sub;
        if( kind == TokenKind::STAR )  return morgana::expr::mul;
        if( kind == TokenKind::SLASH ) return morgana::expr::div;
        return -1;
    };

    nodes.op = (morgana::expr::operand) operand(opdata.kind);

    pContext stmt[2] = { (*ctx)[*index], (opdata.kind == TokenKind::SEMICOLON) ? pContext() : (*ctx)[*index + 2] };

    int exprlen = ((opdata.kind == TokenKind::SEMICOLON) ? 1 : 2);
    *index += ((opdata.kind == TokenKind::SEMICOLON) ? 1 : 3);

    for( int i = 0; i < exprlen; i++ ) {
        pContext side = stmt[i];

        if( side.kind == TokenCtxKind::Block ) {
            pNode result;
            long unsigned int j = 0;

            std::vector<pContext> values = std::get<std::vector<pContext>>(side.content);
            if(! expression(&result, sym, &j, &values, true) ) return false;

            pExpression data = std::get<pExpression>(result.values);
            if( i == 0 ) nodes.lhs = std::make_shared<morgana::expr::nodes>(data.nodes);
            else nodes.rhs = std::make_shared<morgana::expr::nodes>(data.nodes);

            continue;
        }

        if( side.kind == TokenCtxKind::Common ) {
            int pos = std::get<Token>(side.content).kind;
            if( pos >= 39 && pos <= LAST_KEYWORD ) return false;

            if( i == 0 ) nodes.lhs = std::get<Token>(side.content).lexeme;
            else nodes.rhs = std::get<Token>(side.content).lexeme;
        }
    }

    if( exprlen == 1 ) {
        nodes.op = morgana::expr::operand::add;
        nodes.rhs = "0";
    }

    if( internal && *index >= ctx->size() ) {
        expr.nodes = nodes;
        *result = pNode(expr);
        return true;
    }

    pContext check_next = (*ctx)[*index];
    if( check_next.kind == TokenCtxKind::Common ) {
        Token tk = std::get<Token>(check_next.content);

        if( tk.kind == TokenKind::SEMICOLON ) {
            expr.nodes = nodes;
            (*index)++;
            *result = pNode(expr);
            return true;
        }

        int pos = tk.kind;
        if(! (pos >= 7 && pos <= 15) ) return false;

        *index += 1;
        unsigned long int old = *index;
        pNode result;
        if(! expression(&result, sym, index, ctx) ) return false;

        morgana::expr::nodes parent;
        parent.lhs = std::make_shared<morgana::expr::nodes>(nodes);
        parent.op  = (morgana::expr::operand) operand(tk.kind);
        parent.rhs = std::make_shared<morgana::expr::nodes>(std::get<pExpression>(result.values).nodes);

        expr.nodes = parent;
    }

    *result = pNode(expr);
    return true;
}
