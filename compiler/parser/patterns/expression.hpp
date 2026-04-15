#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#define LAST_KEYWORD TokenKind::END_KEYWORDS

template <size_t N>
bool contains(const std::array<std::string, N>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

std::vector<pExpression> exprs;

bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx, bool internal);

int getPrecedence(TokenKind kind) {
    switch(kind) {
        case TokenKind::STAR:
        case TokenKind::SLASH: return 2;
        case TokenKind::PLUS:
        case TokenKind::MINUS: return 1;
        default: return 0;
    }
}

morgana::expr::operand toOperand(TokenKind kind) {
    switch(kind) {
        case TokenKind::PLUS:  return morgana::expr::add;
        case TokenKind::MINUS: return morgana::expr::sub;
        case TokenKind::STAR:  return morgana::expr::mul;
        case TokenKind::SLASH: return morgana::expr::div;
        default: return morgana::expr::add;
    }
}

bool parsePrimary(morgana::expr::nodes& out, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    pContext data = (*ctx)[*index];

    if( data.kind == TokenCtxKind::Block ) {
        long unsigned int j = 0;
        pNode result;

        auto values = std::get<std::vector<pContext>>(data.content);
        if (!expression(&result, sym, &j, &values, true)) return false;

        out = std::get<pExpression>(result.values).nodes;
        (*index)++;
        return true;
    }

    if( data.kind == TokenCtxKind::Common ) {
        Token tk = std::get<Token>(data.content);

        out.lhs = tk.lexeme;
        out.op = morgana::expr::add;
        out.rhs = "0";

        (*index)++;
        return true;
    }

    return false;
}

bool parseBinary(morgana::expr::nodes& lhs,
                 int minPrec,
                 Symt *sym,
                 long unsigned int *index,
                 const std::vector<pContext>* ctx)
{
    while(*index < ctx->size()) {
        pContext opctx = (*ctx)[*index];
        if( opctx.kind != TokenCtxKind::Common ) break;

        Token op = std::get<Token>(opctx.content);
        int prec = getPrecedence(op.kind);
        if( prec < minPrec ) break;

        (*index)++;

        morgana::expr::nodes rhs;
        if(! parsePrimary(rhs, sym, index, ctx) ) return false;

        while(*index < ctx->size()) {
            pContext next = (*ctx)[*index];
            if( next.kind != TokenCtxKind::Common ) break;

            Token nextOp = std::get<Token>(next.content);
            int nextPrec = getPrecedence(nextOp.kind);

            if( nextPrec > prec ) {
                if(! parseBinary(rhs, nextPrec, sym, index, ctx) ) return false;
            } else break;
        }

        morgana::expr::nodes parent;
        parent.op = toOperand(op.kind);
        parent.lhs = std::make_shared<morgana::expr::nodes>(lhs);
        parent.rhs = std::make_shared<morgana::expr::nodes>(rhs);

        lhs = parent;
    }

    return true;
}

bool expression(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx, bool internal=false) {
    pExpression expr;

    if( (*index) < ctx->size() ) {
        auto& data = (*ctx)[*index];
        if (data.kind == TokenCtxKind::Common) {
            std::string lex = std::get<Token>(data.content).lexeme;
            if( lex == "try" || lex == "if" || lex == "gpio" ) {
                expr.keyword = lex;
                (*index)++;
            }
        }
    }

    morgana::expr::nodes root;
    if(! parsePrimary(root, sym, index, ctx) ) return false;
    if(! parseBinary(root, 1, sym, index, ctx) ) return false;

    if( (*index) < ctx->size() ) {
        auto& data = (*ctx)[*index];
        if( data.kind == TokenCtxKind::Common ) {
            std::string lex = std::get<Token>(data.content).lexeme;
            if( lex == "else" ) {
                (*index)++;
            }
        }
    }

    if(! internal && (*index) < ctx->size() ) {
        auto& data = (*ctx)[*index];
        if( data.kind == TokenCtxKind::Common ) {
            if( std::get<Token>(data.content).kind == TokenKind::SEMICOLON ) {
                (*index)++;
            }
        }
    }

    expr.nodes = root;
    *result = pNode(expr);
    return true;
}
