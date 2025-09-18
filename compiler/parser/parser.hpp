#include <cstddef>
#include <string>
#include <vector>
#include "ast.hpp"
#include "pattern.hpp"
#include "symbols.hpp"
#include "../tokenizer/token.hpp"
#include "../tokenizer/token_kind.hpp"
#include "../debug/debug.hpp"
#include "../utils/result.hpp"
#include "../compiler_outputs.hpp"

#define ips iPerScope[scope]

typedef struct Parser {
private:
    static std::vector<pContext> genCTX(std::vector<Token>& tks);
    static Result checkSyntax(std::vector<pContext>& ctx, std::vector<Token>& tks);

    /* Utils functions */
    static bool isEOF(Token tk);
public:
// std::vector<pNode> parse(std::vector<Token>& tks);
    static void parse(std::vector<Token>& tks);
} Parser;

inline bool Parser::isEOF(Token tk) {
    return tk.kind == CARLA_EOF;
}

void Parser::parse(std::vector<Token>& tks) {
    std::vector<pContext> ctx = genCTX(tks);

#   if DEBUG
    for (size_t i = 0; i < ctx.size(); ++i) {
        printCtx(ctx[i], "", i == ctx.size() - 1);
    }
#   endif

    Result syntax = checkSyntax(ctx, tks);
    if(! isSuccess(syntax) ) return CompilerOutputs::Fatal(err(syntax));
}

Result Parser::checkSyntax(std::vector<pContext>& ctx, std::vector<Token>& tks) {
    Symbols symbols;

    std::vector<pNode> nodes;
    std::vector<std::pair<const std::vector<pContext>*, size_t>> stack;
    stack.emplace_back(&ctx, 0);

    int currentDepth = 0;
    size_t globalIndex = 0;

    while(!stack.empty()) {
        auto& [currentCtx, index] = stack.back();

        if( index >= currentCtx->size() ) {
            stack.pop_back();
            if (currentDepth > 0) currentDepth--;
            symbols.exitScope();
            continue;
        }

        const pContext& context = (*currentCtx)[index];

#       if DEBUG
        std::cout << "Global: " << globalIndex++
                  << ", Depth: " << currentDepth
                  << ", Type: " << (context.kind == Block ? "Block" : "Common")
                  << std::endl;
#       endif

        if (context.kind == Common) {
            Token tk = std::get<Token>(context.content);
            pNode node;
            Result match = pattern(&node, &index, currentCtx);
            if(! isSuccess(match) ) CompilerOutputs::Fatal(err(match));
            nodes.push_back(node);
        }

        index++;

        if (context.kind == Block) {
            const auto& blockContent = std::get<std::vector<pContext>>(context.content);
            if (!blockContent.empty()) {
                stack.emplace_back(&blockContent, 0);
                currentDepth++;
                symbols.enterScope();
            }
        }
    }

    return Some{};
}

std::vector<pContext> Parser::genCTX(std::vector<Token>& tks) {
    std::vector<std::vector<pTokenCtx>> contextStack;
    contextStack.emplace_back();

    int i = 0;
    while(! isEOF(tks[i]) ) {
        Token tk = tks[i++];
        TokenSubKind sub = getSub(tk.kind);

        switch(sub) {
            case OPEN_CONTEXT: {
                contextStack.emplace_back();
            } break;

            case CLOSE_CONTEXT: {
                if( contextStack.size() <= 1 ) CompilerOutputs::Fatal("Unexpected CLOSE_CONTEXT with no matching OPEN_CONTEXT");

                std::vector<pTokenCtx> closedBlock = std::move(contextStack.back());
                contextStack.pop_back();

                pTokenCtx blockCtx(Block, std::move(closedBlock));

                contextStack.back().emplace_back(std::move(blockCtx));
            } break;

            case UNKNOWN: {
                contextStack.back().emplace_back(Common, tk);
            } break;
        }
    }

    if( contextStack.size() != 1 ) CompilerOutputs::Fatal("Unclosed block(s) detected at end of input");

    return contextStack.back();
}
