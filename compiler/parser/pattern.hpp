// Faz o Handler dos possiveis Patterns

#pragma once

#include "../utils/result.hpp"
#include "ast.hpp"
#include "patterns/expression.hpp"
#include "patterns/lambda.hpp"
#include "patterns/declaration.hpp"
#include "patterns/lambda.hpp"
#include "../compiler_outputs.hpp"
#include "../tokenizer/token_kind.hpp"
#include <sstream>

template<typename T, typename X>
Result pattern(pNode *result, Symt *sym, T index, X ctx) {
    const pContext& context = (*ctx)[*index];
    if( context.kind == Block ) {
        if( lambda(result, sym, index, ctx) ) return Some{};
        // else if( expression(result, sym, index, ctx) ) return Some{};
        else return Err{unknownPattern(ctx, index)};
    }

    Token tk = std::get<Token>(context.content);

    switch(tk.kind) {
    case IDENTIFIER:
    if( declaration(result, sym, index, ctx) ) return Some{};
    case NUMBER:
    if( expression(result, sym, index, ctx) ) return Some{};
    default: return Err{unknownPattern(ctx, index)};
    }

    return Err{unknownPattern(ctx, index)};
}

template<typename T, typename X>
std::string unknownPattern(const T ctx, const X index) {
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
