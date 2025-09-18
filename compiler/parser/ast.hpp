// Isto define as estruturas geradas pelo parser.hpp

#pragma once

#include "../tokenizer/token.hpp"
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
  NODE_FUNCTION,
  NODE_CALL
};

struct pNode { NodeKind kind; };
