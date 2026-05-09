#pragma once

#include "../pattern.hpp"
#include "../nodes/call.hpp"
#include "expression.hpp"

bool call(CARLA_PATTERN_ARGUMENTS) {
    CARLA_PATTERN_STARTS(bool, false);
    CARLA_GET_NEXT(identifier, _default);
    if( identifier.kind != Common ) CARLA_RETURN_DEFAULT;

    auto identifier_tk = std::get<Token>(identifier.content);
    auto identifier_data = identifier_tk.lexeme;

    CARLA_GET_NEXT(arguments, _default);
    if( arguments.kind != Block ) CARLA_RETURN_DEFAULT;
    auto arguments_data = std::get<std::vector<pTokenCtx>>(arguments.content);

    size_t i = 0;
    std::vector<carla::Expr> args;
    while(i < arguments_data.size()) {
        pNode node;
        if(! expression(&node, sym, &i, &arguments_data) ) CARLA_RETURN_DEFAULT;
        args.push_back(std::get<carla::Expr>(node));
    }

    carla::Type r("int8*", morgana::ptr());
    carla::Call call(identifier_data, args, r);
    *result = call;

    return true;
}
