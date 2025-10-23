#pragma once

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

struct Parser {
private:
    static std::vector<pContext> genCTX(std::vector<Token>& tks);

    /* Utils functions */
    static bool isEOF(Token tk);
public:
// std::vector<pNode> parse(std::vector<Token>& tks);
    static std::vector<pNode> parse(Symbols& symbols, std::vector<Token>& tks);
    static Result checkSyntax(Symbols& symbols, std::vector<pNode> *nodes,std::vector<pContext>& ctx, bool global);
};

inline bool Parser::isEOF(Token tk) {
    return tk.kind == CARLA_EOF;
}

std::vector<pNode> Parser::parse(Symbols& symbols, std::vector<Token>& tks) {
    std::vector<pContext> ctx = genCTX(tks);

#   if CARLA_DEBUG
    for( size_t i = 0; i < ctx.size(); ++i ) {
        printCtx(ctx[i], "", i == ctx.size() - 1);
    }
#   endif

    std::vector<pNode> nodes;
    Result syntax = checkSyntax(symbols, &nodes, ctx, true);
    if(! isSuccess(syntax) ) CompilerOutputs::Fatal(err(syntax));
    return nodes;
}

Result Parser::checkSyntax(Symbols& symbols, std::vector<pNode> *nodes, std::vector<pContext>& ctx, bool global) {
    std::vector<std::pair<const std::vector<pContext>*, size_t>> stack;
    stack.emplace_back(&ctx, 0);

    int currentDepth = 0;
    size_t globalIndex = 0;

    int skip = 0;

    while(!stack.empty()) {
        auto& [currentCtx, index] = stack.back();

        if( skip > 0 ) {
            skip--;
            index++;
            if( skip == 0 ) index--;
        }

        if( index >= currentCtx->size() ) {
            stack.pop_back();
            if (currentDepth > 0) currentDepth--;
            symbols.exitScope();
            continue;
        }

        const pContext& context = (*currentCtx)[index];

        #if CARLA_DEBUG
        std::cout << "Global: " << globalIndex++
                  << ", Depth: " << currentDepth
                  << ", Type: " << (context.kind == Block ? "Block" : "Common")
                  << std::endl;
        #endif

        pNode node = pNode();
        int old = index;
        Result match = pattern(&node, &symbols, &index, currentCtx);
        if( isSuccess(match) ) {
            nodes->push_back(node);
            skip = index - old;
            continue;
        } else {
            CompilerOutputs::Fatal(err(match));
        }

        if( context.kind == Block ) {
            const auto& blockContent = std::get<std::vector<pContext>>(context.content);
            if(! blockContent.empty() ) {
                stack.emplace_back(&blockContent, 0);
                currentDepth++;
                symbols.enterScope();
            }
            index++;
            continue;
        }

        CompilerOutputs::Fatal(err(match));
    }


#   if CARLA_DEBUG
    printNodes(*nodes);
#   endif

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
