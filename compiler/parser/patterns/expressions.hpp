#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <stack>

#include "lambda.hpp"

// Adiciona enum para precedência de operadores
enum OperatorPrecedence : int {
    PREC_LOWEST = 0,
    PREC_ASSIGNMENT = 1,   // =, +=, -=, *=, /=, etc.
    PREC_LOGICAL_OR = 2,   // ||
    PREC_LOGICAL_AND = 3,  // &&
    PREC_EQUALITY = 4,     // ==, !=
    PREC_RELATIONAL = 5,   // <, >, <=, >=
    PREC_BITWISE_OR = 6,   // |
    PREC_BITWISE_XOR = 7,  // ^
    PREC_BITWISE_AND = 8,  // &
    PREC_SHIFT = 9,        // <<, >>
    PREC_ADDITIVE = 10,    // +, -
    PREC_MULTIPLICATIVE = 11, // *, /, %
    PREC_UNARY = 12,       // !, ~, +, -, ++, --
    PREC_CALL = 13,        // (), []
    PREC_MEMBER = 14       // ., ->
};

// Mapeamento de operadores para sua precedência
std::map<TokenKind, OperatorPrecedence> operatorPrecedence = {
    {PLUS, PREC_ADDITIVE},
    {MINUS, PREC_ADDITIVE},
    {STAR, PREC_MULTIPLICATIVE},
    {SLASH, PREC_MULTIPLICATIVE},
    {EQUAL, PREC_EQUALITY},
    // {NOT_EQUAL, PREC_EQUALITY},
    // {LESS_THAN, PREC_RELATIONAL},
    // {GREATER_THAN, PREC_RELATIONAL},
    {LESS_EQUAL, PREC_RELATIONAL},
    {GREATER_EQUAL, PREC_RELATIONAL},
    {AND, PREC_LOGICAL_AND},
    {OR, PREC_LOGICAL_OR},
    // {ASSIGN, PREC_ASSIGNMENT},
    // {PLUS_ASSIGN, PREC_ASSIGNMENT},
    // {MINUS_ASSIGN, PREC_ASSIGNMENT},
    // {STAR_ASSIGN, PREC_ASSIGNMENT},
    // {SLASH_ASSIGN, PREC_ASSIGNMENT},
    // {MOD, PREC_MULTIPLICATIVE},
    // {BIT_AND, PREC_BITWISE_AND},
    // {BIT_OR, PREC_BITWISE_OR},
    // {BIT_XOR, PREC_BITWISE_XOR},
    // {BIT_NOT, PREC_UNARY},
    // {LEFT_SHIFT, PREC_SHIFT},
    // {RIGHT_SHIFT, PREC_SHIFT},
    {DOT, PREC_MEMBER},
    {ARROW, PREC_MEMBER},
    {LEFT_PAREN, PREC_CALL}
};

bool parseExpression(std::shared_ptr<pNode>* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx,
                    OperatorPrecedence minPrecedence);

bool parseExpressionBlock(pNode* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx);

// Função para obter precedência do operador
OperatorPrecedence getPrecedence(TokenKind kind) {
    auto it = operatorPrecedence.find(kind);
    if (it != operatorPrecedence.end()) {
        return it->second;
    }
    return PREC_LOWEST;
}

// Função para verificar se o token é um operador
bool isOperator(TokenKind kind) {
    return operatorPrecedence.find(kind) != operatorPrecedence.end();
}

// Função para analisar uma expressão primária (literal, variável, etc.)
bool parsePrimaryExpression(std::shared_ptr<pNode>* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx) {
    if (*index >= ctx->size()) return false;

    const pContext& context = (*ctx)[*index];

    // Token simples (identificador, número, string)
    if (context.kind == Common && std::holds_alternative<Token>(context.content)) {
        Token token = std::get<Token>(context.content);

        if (token.kind == IDENTIFIER) {
            if (!sym->contains(token.lexeme)) return false;

            Symbol symbol = sym->get(token.lexeme);
            sym->markAsUsed(token.lexeme); // Marca o símbolo como usado

            pExpression expr(EXPR_VARIABLE, symbol);
            *result = std::make_shared<pNode>(expr);
            (*index)++;
            return true;
        }
        else if (token.kind == NUMBER) {
            long long val = std::stoll(token.lexeme);
            pExpression expr(EXPR_LITERAL, val);
            *result = std::make_shared<pNode>(expr);
            (*index)++;
            return true;
        }
        else if (token.kind == STRING) {
            pExpression expr(EXPR_LITERAL, token.lexeme);
            *result = std::make_shared<pNode>(expr);
            (*index)++;
            return true;
        }
    }
    // Expressão em parenteses
    else if (context.kind == Common && std::holds_alternative<Token>(context.content) &&
             std::get<Token>(context.content).kind == LEFT_PAREN) {
        (*index)++; // Consumir '('

        std::shared_ptr<pNode> innerExpr;
        if (!parseExpression(&innerExpr, sym, index, ctx, PREC_LOWEST)) {
            return false;
        }

        if (*index >= ctx->size() ||
            !std::holds_alternative<Token>((*ctx)[*index].content) ||
            std::get<Token>((*ctx)[*index].content).kind != RIGHT_PAREN) {
            // Erro: esperava ')'
            return false;
        }

        (*index)++; // Consumir ')'
        *result = innerExpr;
        return true;
    }
    // Bloco (lambda ou subexpressão)
    else if (context.kind == Block) {
        pNode blockResult;
        size_t tmpIndex = 0;

        if (lambda(&blockResult, sym, &tmpIndex, &std::get<std::vector<pContext>>(context.content))) {
            *result = std::make_shared<pNode>(blockResult);
            (*index)++;
            return true;
        }
        else if (parseExpressionBlock(&blockResult, sym, &tmpIndex, &std::get<std::vector<pContext>>(context.content))) {
            *result = std::make_shared<pNode>(blockResult);
            (*index)++;
            return true;
        }
    }

    return false;
}

// Função para analisar um bloco de expressão
bool parseExpressionBlock(pNode* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx) {
    sym->enterScope(); // Entrar em novo escopo para o bloco

    size_t internal = *index;
    std::vector<std::shared_ptr<pNode>> exprs;

    while (internal < ctx->size()) {
        std::shared_ptr<pNode> expr;
        if (!parseExpression(&expr, sym, &internal, ctx, PREC_LOWEST)) {
            sym->exitScope();
            return false;
        }

        exprs.push_back(expr);

        // Verificar por ';' no final da expressão
        if (internal < ctx->size() &&
            std::holds_alternative<Token>((*ctx)[internal].content) &&
            std::get<Token>((*ctx)[internal].content).kind == SEMICOLON) {
            internal++; // Consumir ';'
        }
    }

    // O resultado é a última expressão do bloco
    if (exprs.empty()) {
        sym->exitScope();
        return false;
    }

    *result = *exprs.back();
    *index = internal;
    sym->exitScope();
    return true;
}

// Função para analisar uma expressão binária
bool parseBinaryExpression(std::shared_ptr<pNode>* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx,
                           std::shared_ptr<pNode> left, TokenKind opKind, OperatorPrecedence precedence) {
    (*index)++; // Consumir o operador

    std::shared_ptr<pNode> right;
    if (!parseExpression(&right, sym, index, ctx, precedence)) {
        return false;
    }

    pExpression expr(EXPR_BINARY, std::make_tuple(opKind, left, right));
    *result = std::make_shared<pNode>(expr);
    return true;
}

// Função principal para analisar expressões com precedência
bool parseExpression(std::shared_ptr<pNode>* result, Symbols* sym, size_t* index, const std::vector<pContext>* ctx,
                    OperatorPrecedence minPrecedence) {
    std::shared_ptr<pNode> left;
    if (!parsePrimaryExpression(&left, sym, index, ctx)) {
        return false;
    }

    while (*index < ctx->size()) {
        const pContext& context = (*ctx)[*index];

        // Verifique se é o fim da expressão
        if (context.kind == Common && std::holds_alternative<Token>(context.content)) {
            Token token = std::get<Token>(context.content);

            if (token.kind == SEMICOLON || token.kind == RIGHT_PAREN ||
                token.kind == COMMA || token.kind == RIGHT_BRACKET) {
                break;
            }

            // Verifique se é um operador
            if (isOperator(token.kind)) {
                OperatorPrecedence currentPrecedence = getPrecedence(token.kind);

                if (currentPrecedence < minPrecedence) {
                    break;
                }

                // Para operadores de mesma precedência, associatividade da esquerda para a direita
                OperatorPrecedence nextPrecedence =
                    (currentPrecedence == PREC_ASSIGNMENT) ? currentPrecedence : currentPrecedence + 1;

                if (!parseBinaryExpression(&left, sym, index, ctx, left, token.kind, nextPrecedence)) {
                    return false;
                }
                continue;
            }
        }

        break;
    }

    *result = left;
    return true;
}

// Função principal para expressões
bool expressions(pNode* result, Symbols* sym, long unsigned int* index, const std::vector<pContext>* ctx) {
    std::shared_ptr<pNode> expr;
    size_t internal = *index;

    if (!parseExpression(&expr, sym, &internal, ctx, PREC_LOWEST)) {
        return false;
    }

    // Verificar por ';' no final da expressão
    if (internal < ctx->size() &&
        (*ctx)[internal].kind == Common &&
        std::holds_alternative<Token>((*ctx)[internal].content) &&
        std::get<Token>((*ctx)[internal].content).kind == SEMICOLON) {
        internal++; // Consumir ';'
    }

    *result = *expr;
    *index = internal;
    return true;
}
