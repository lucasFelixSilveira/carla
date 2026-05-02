#pragma once

#include <variant>
#include <vector>
#include "../tokenizer/token.hpp"

enum TokenCtxKind { Common, Block };
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
