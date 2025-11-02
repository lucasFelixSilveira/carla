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
    NODE_CALL,

    NODE_EXPRESSION
};

inline std::string pKindStr(NodeKind kind) {
    switch (kind) {
        case NODE_DECLARATION: return "NODE_DECLARATION";
        case NODE_LAMBDA:      return "NODE_LAMBDA";
        case NODE_CALL:        return "NODE_CALL";
        case NODE_EXPRESSION:  return "NODE_EXPRESSION";
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
    pContext body;

    morgana::desconstruct::values argsn;
    morgana::function::args argst;

    pLambda(bool pub, morgana::function::args argst, morgana::desconstruct::values argsn, pContext body)
        : pub(pub), argst(std::move(argst)), argsn(std::move(argsn)), body(body) {}
};

struct pExpression {
    std::variant<double, std::string, std::shared_ptr<pNode>> left;
    std::string op;
    std::variant<double, std::string, std::shared_ptr<pNode>> right;
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
    pNode(bool pub, morgana::function::args argst, morgana::desconstruct::values argsn, pContext body)
        : kind(NODE_LAMBDA), values(pLambda(pub, std::move(argst), std::move(argsn), body)) {}

    // expressão
    pNode(const pExpression& expr)
        : kind(NODE_EXPRESSION), values(expr) {}
    pNode() = default;
};
