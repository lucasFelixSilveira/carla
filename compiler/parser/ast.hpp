// Isto define as estruturas geradas pelo parser.hpp

#pragma once

#include "../tokenizer/token.hpp"
#include "symbols.hpp"
#include <string>
#include <variant>
#include <vector>
#include <memory>

// --------------------
// Contextos de tokens
// --------------------
enum TokenCtxKind {
    Common,
    Block
};

struct pTokenCtx;
using TokenContent = std::variant<Token, std::vector<pTokenCtx>>;

struct pTokenCtx {
    TokenCtxKind kind;
    TokenContent content;

    pTokenCtx(TokenCtxKind k, const Token& t)
        : kind(k), content(t) {}

    pTokenCtx(TokenCtxKind k, std::vector<pTokenCtx> b)
        : kind(k), content(std::move(b)) {}
};

using pContext = pTokenCtx;

// --------------------
// Tipos de nós da AST
// --------------------
enum NodeKind {
    NODE_DECLARATION,
    NODE_LAMBDA,
    NODE_EXPRESSION,
    NODE_CALL,
};

inline std::string pKindStr(NodeKind kind) {
    switch (kind) {
        case NODE_DECLARATION: return "NODE_DECLARATION";
        case NODE_LAMBDA:      return "NODE_LAMBDA";
        case NODE_EXPRESSION:  return "NODE_EXPRESSION";
        case NODE_CALL:        return "NODE_CALL";
        default:               return "UNKNOWN";
    }
}

// --------------------
// Declarações
// --------------------
enum pDeclaration_e {
    dIterable,
    dHopeless,
    dHopefull,
    dArgument,
};

inline std::string pDeclarationStr(pDeclaration_e kind) {
    switch (kind) {
        case dIterable: return "dIterable";
        case dHopeless: return "dHopeless";
        case dHopefull: return "dHopefull";
        case dArgument: return "dArgument";
        default:        return "UNKNOWN";
    }
}

struct pDeclaration {
    std::string name;
    Symbol type;
    pDeclaration_e complement;

    pDeclaration(pDeclaration_e dType, Symbol type, const std::string& identifier)
        : name(identifier), type(type), complement(dType) {}
};

// --------------------
// Estruturas de lambda
// --------------------
struct pNode; // forward

struct pLambda {
    bool pub;
    std::vector<pNode> args;

    pLambda(bool pub, std::vector<pNode> args)
        : pub(pub), args(std::move(args)) {}
};

// --------------------
// Expressões
// --------------------
enum ExprKind {
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_CALL,
    EXPR_MEMBER,
    EXPR_INDEX,
    EXPR_TERNARY,
    EXPR_GROUP
};

// Agora usamos ponteiros para permitir recursão infinita
using ExprContent = std::variant<
    std::tuple<TokenKind, std::shared_ptr<pNode>, std::shared_ptr<pNode>>,            // operador binário
    std::tuple<TokenKind, std::shared_ptr<pNode>>,                                    // operador unário
    std::variant<std::string, long long int, double, bool>,                           // literal
    Symbol,                                                                           // variável
    std::tuple<std::shared_ptr<pNode>, std::vector<std::shared_ptr<pNode>>>,          // chamada de função
    std::tuple<std::shared_ptr<pNode>, std::string>,                                  // acesso a membro
    std::tuple<std::shared_ptr<pNode>, std::shared_ptr<pNode>>,                       // acesso a índice
    std::tuple<std::shared_ptr<pNode>, std::shared_ptr<pNode>, std::shared_ptr<pNode>>, // operador ternário
    std::shared_ptr<pNode>                                                            // expressão agrupada
>;

struct pExpression {
    ExprKind kind;
    ExprContent content;

    pExpression(ExprKind kind, ExprContent content)
        : kind(kind), content(std::move(content)) {}
};

// --------------------
// Nó da AST
// --------------------
using pValues = std::variant<
    std::monostate,
    pDeclaration,
    pLambda,
    pExpression
>;

struct pNode {
    NodeKind kind;
    pValues values;

    // declaração
    pNode(pDeclaration_e dType, Symbol type, const std::string& identifier)
        : kind(NODE_DECLARATION), values(pDeclaration(dType, type, identifier)) {}

    // lambda
    pNode(bool pub, std::vector<pNode> args)
        : kind(NODE_LAMBDA), values(pLambda(pub, std::move(args))) {}

    // expressão
    pNode(pExpression expr)
        : kind(NODE_EXPRESSION), values(std::move(expr)) {}

    pNode() = default;
};
