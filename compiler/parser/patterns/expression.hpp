#pragma once

#include "../pattern.hpp"
#include "../nodes/expression.hpp"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

size_t precedence(TokenKind kind) {
    switch(kind) {
        case TokenKind::PLUS:
        case TokenKind::MINUS: return 1;
        case TokenKind::STAR:
        case TokenKind::SLASH: return 2;
        case TokenKind::SHIFT_LEFT:
        case TokenKind::SHIFT_RIGHT:
        case TokenKind::LOR:
        case TokenKind::LAND:
        case TokenKind::XOR: return 3;
        default: return 0;
    }
}

bool is_operator(TokenKind kind)
{
    return kind == PLUS || kind == MINUS || kind == STAR || kind == SLASH || kind == SLASH_SLASH // Arithmetic operators
        || kind == SHIFT_LEFT || kind == SHIFT_RIGHT // Binary shift operators
        || kind == LOR || kind == LAND || kind == XOR; // Binary operators
};

bool is_expr_end_keyword(TokenKind kind)
{ return kind == TokenKind::ELSE; }

std::tuple<bool, carla::AST> make_ast(CARLA_PATTERN_ARGUMENTS);
bool expression(CARLA_PATTERN_ARGUMENTS) {
    CARLA_PATTERN_STARTS(bool, false);
    CARLA_PEEK_NEXT(first, _default);
    auto [success, ast] = make_ast(CARLA_PATTERN_EXPORT);
    if(! success ) CARLA_RETURN_DEFAULT;
}

void reorder(std::vector<pContext>& sub) {
    std::vector<pContext> original = sub;
    sub.clear();

    std::vector<std::tuple<size_t, size_t>> op_precedence;

    std::vector<pContext> reordered;
    for( int i = 0; i < original.size(); i++ ) {
        auto& node = original.at(i);
        if( node.kind == Block ) continue;
        auto tk = std::get<Token>(node.content).kind;

        if(! is_operator(tk) ) continue;
        op_precedence.push_back({ i, precedence(tk) });
    }

    std::sort(op_precedence.begin(), op_precedence.end(),
        []( const auto& a, const auto& b ) {
            if( std::get<0>(a) != std::get<0>(b) )
            /* -> */ return std::get<0>(a) > std::get<0>(b);
            return std::get<1>(a) < std::get<1>(b);
        }
    );

    for( auto& [i, p] : op_precedence ) {
        // std::cout << "i: " << i << ", p: " << p << std::endl;
        reordered.push_back(original[i]);
    }
    sub = reordered;
}

std::tuple<bool, carla::AST> make_ast(CARLA_PATTERN_ARGUMENTS) {
    auto d = std::make_tuple(false, carla::AST());
    CARLA_PATTERN_STARTS(auto, d);

    carla::AST ast;
    std::vector<pContext> sub;
    bool endded = false;
    while((*index) < ctx->size()) {
        auto node = (*ctx)[(*index)++];
        if(
            node.kind == Common
            && std::get<Token>(node.content).kind == TokenKind::SEMICOLON
        ) {
            endded = true;
            break;
        }
        sub.push_back(node);
    }

    if(! endded ) CARLA_RETURN_DEFAULT;
    if( sub.empty() ) CARLA_RETURN_DEFAULT;

    reorder(sub);
    return { true, ast };
}
