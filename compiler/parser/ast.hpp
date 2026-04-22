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

    pTokenCtx() = default;
};

using pContext = pTokenCtx;

// --------------------
// Tipos de nós da AST
// --------------------
enum NodeKind {
    NODE_DECLARATION,
    NODE_LAMBDA,
    NODE_CALL,

    NODE_EXPRESSION,

    NODE_MACRO,
    NODE_RETURN,
    NODE_PUTS
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
    dUknownType
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
    std::vector<pContext> ctx;
    int kind_definition;

    pDeclaration(pDeclaration_e dType, Symbol type, const std::string& identifier, std::vector<pContext> ctx, int kind_definition)
        : name(identifier), type(type), complement(dType), ctx(ctx), kind_definition(kind_definition) {}
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
    std::string name;
    morgana::expr::root root;
    std::vector<pContext> ctx;

    pExpression(const std::string& name, morgana::expr::root root, std::vector<pContext> ctx)
        : name(name), root(root), ctx(ctx) {}
};

struct Macro {
    using values_t = std::variant<std::monostate, std::string>;
    values_t values;
    enum class options { start, extern_ };
    options kind;

    Macro(options opt, values_t values=std::monostate()) : kind(opt), values(values) {}

    static Macro start() {
        return Macro(Macro::options::start);
    }

    static Macro extern_(std::string data) {
        return Macro(Macro::options::extern_, data);
    }
};

struct SimpleStatement {
    bool hopeless;
    SimpleStatement(bool hopeless) : hopeless(hopeless) {}
};

// --------------------
// Nó da AST
// --------------------
using pValues = std::variant<
    std::monostate,
    pDeclaration,
    pLambda,
    pExpression,
    Macro,
    SimpleStatement
>;


struct pNode {
    NodeKind kind;
    pValues values;

    // declaração
    pNode(pDeclaration_e dType, Symbol type, const std::string& identifier, std::vector<pContext> ctx, int kind_definition = -1)
        : kind(NODE_DECLARATION), values(pDeclaration(dType, type, identifier, ctx, kind_definition)) {}

    // lambda
    pNode(bool pub, morgana::function::args argst, morgana::desconstruct::values argsn, pContext body)
        : kind(NODE_LAMBDA), values(pLambda(pub, std::move(argst), std::move(argsn), body)) {}

    // expressão
    pNode(pExpression& expr)
        : kind(NODE_EXPRESSION), values(expr) {}

    pNode(const Macro macro)
        : kind(NODE_MACRO), values(macro) {}

    pNode(const NodeKind kind, const SimpleStatement ret)
        : kind(kind), values(ret) {}

    pNode() = default;
};
