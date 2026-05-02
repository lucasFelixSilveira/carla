// Faz o Handler dos possiveis Patterns

#pragma once

#include "ctx.hpp"
#include "node.hpp"
#include "symbols.hpp"
#include <vector>

#define CARLA_PATTERN_ARGUMENTS pNode *result, Symt *sym, size_t *index, const std::vector<pContext>* ctx
#define CARLA_PATTERN_EXPORT result, sym, index, ctx
#define CARLA_PATTERN_DECLINE { (*index) = backup; return false; }
#define CARLA_PATTERN_STARTS(return_t, data) size_t backup = *index; \
                                             return_t _default = data;

#define CARLA_RETURN_DEFAULT { (*index) = backup; return _default; }

#define CARLA_GET_NEXT(id, val) if( *index >= ctx->size() ) return val; \
                                auto id = (*ctx)[(*index)++]

#define CARLA_INDEX_NEXT(id, val, ctx, index) if( *index >= ctx->size() ) return val; \
                                              auto id = (*ctx)[(*index)++]


#define CARLA_PEEK_NEXT(id, val) if( *index >= ctx->size() ) return val; \
                                  auto id = (*ctx)[*index]

#include "../utils/result.hpp"
#include "../compiler_outputs.hpp"
#include "../tokenizer/token_kind.hpp"

#include "./patterns/declaration.hpp"
#include "patterns/macros.hpp"
#include "./patterns/lambda.hpp"

#include <cstddef>
#include <sstream>

std::string unknownPattern(const std::vector<pContext>* ctx, size_t *index);

Result pattern(CARLA_PATTERN_ARGUMENTS) {
    const pContext& context = (*ctx)[*index];
    if( context.kind == Block ) {
        if( lambda(CARLA_PATTERN_EXPORT) ) return Some{};
        else if( declaration(CARLA_PATTERN_EXPORT) ) return Some{};
        else return Err{unknownPattern(ctx, index)};
    }

    Token tk = std::get<Token>(context.content);

    switch(tk.kind) {
    case START:
    if( macros(CARLA_PATTERN_EXPORT, tk.kind) ) return Some{};
    // case RETURN:
    // if( returnStatement(result, sym, index, ctx) ) return Some{};
    // case PUTS:
    // if( putsStatement(result, sym, index, ctx) ) return Some{};
    // // case LET:
    // // case _CONST:
    // // if( keywordDeclaration(result, sym, index, ctx) ) return Some{};
    case IDENTIFIER:
    if( declaration(CARLA_PATTERN_EXPORT) ) return Some{};
    // else if( expression(result, sym, index, ctx, true) ) return Some{};
    // case NUMBER:
    // if( expression(result, sym, index, ctx, true) ) return Some{};
    // case STRING:
    // if( expression(result, sym, index, ctx, true) ) return Some{};
    default: return Err{unknownPattern(ctx, index)};
    }

    return Err{unknownPattern(ctx, index)};
}

std::string unknownPattern(const std::vector<pContext>* ctx, size_t *index) {
    std::stringstream str;
    std::stringstream buff;
    std::stringstream line;

    const pContext& context = (*ctx)[*index];
    if( context.kind == Common ) {
        Token tk = std::get<Token>(context.content);
        buff << ((tk.lexeme.length() == 0) ? tokenKindToString(tk.kind) : tk.lexeme);
        line << std::to_string(tk.line);
    } else {
        buff << Colorizer::BOLD_YELLOW << "Carla[Internal<Block>]" << Colorizer::RESET;
        line << Colorizer::BOLD_YELLOW << "Carla[Internal<Line(?:Numeric!)>]" << Colorizer::RESET;
    }

    str << Colorizer::RED << "Unknown pattern at context index " << *index << " (addr. " << Colorizer::GREEN << index << Colorizer::RED << ')' << Colorizer::RESET << ": '" << buff.str() << "'\n";
    str << Colorizer::DARK_GREY << "└─ " << Colorizer::RESET << "Expected another pattern at line " << line.str() << "\n";
    return str.str();
}
