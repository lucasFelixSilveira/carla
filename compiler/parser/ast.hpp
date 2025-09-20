// Isto define as estruturas geradas pelo parser.hpp

#pragma once

#include "../tokenizer/token.hpp"
#include "symbols.hpp"
#include <any>
#include <string>
#include <variant>
#include <vector>

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

enum NodeKind {
  NODE_DECLARATION,
  NODE_LAMBDA,
  NODE_CALL
};


std::string pKindStr(NodeKind kind) {
    switch (kind) {
        case NodeKind::NODE_DECLARATION: return "NODE_DECLARATION";
        case NodeKind::NODE_LAMBDA: return "NODE_LAMBDA";
        default: return "UNKNOWN";
    }
}

enum pDeclaration_e {
    dIterable,
    dHopeless,
    dHopefull,
    dArgument,
};

std::string pDeclarationStr(pDeclaration_e kind) {
    switch (kind) {
        case dHopefull: return "dHopefull";
        case dArgument: return "dArgument";
        default: return "UNKNOWN";
    }
}

struct pDeclaration {
    std::string name;
    Symbol type;
    pDeclaration_e complement;

    pDeclaration(pDeclaration_e dType, Symbol type, const std::string& identifier)
        : name(identifier), type(type), complement(dType) {}
};

struct pNode;

struct pLambda {
    bool pub;
    std::vector<pNode> args;

    pLambda(bool pub, std::vector<pNode> args)
        : pub(pub), args(std::move(args)) {}
};

using pValues = std::variant<std::monostate, pDeclaration, pLambda>;

struct pNode {
    NodeKind kind;
    pValues values;

    pNode(pDeclaration_e dType, Symbol type, const std::string& identifier)
        : kind(NodeKind::NODE_DECLARATION), values(pDeclaration(dType, type, identifier)) {}

    pNode(bool pub, std::vector<pNode> args)
        : kind(NodeKind::NODE_LAMBDA), values(pLambda(pub, std::move(args))) {}

    pNode() = default;
};
