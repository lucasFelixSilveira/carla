#pragma once
#include "type.hpp"
#include "../nodes/declaration.hpp"
#include "../pattern.hpp"

bool declaration(CARLA_PATTERN_ARGUMENTS) {
    CARLA_PATTERN_STARTS(bool, false);
    auto [success, type, str] = typement(CARLA_PATTERN_EXPORT);
    if(! success ) CARLA_PATTERN_DECLINE;

    CARLA_GET_NEXT(id, _default);
    if( id.kind != Common ) CARLA_RETURN_DEFAULT;

    auto tk = std::get<Token>(id.content);
    if( tk.kind != IDENTIFIER ) CARLA_RETURN_DEFAULT;
    std::string identifier = tk.lexeme;

    CARLA_GET_NEXT(equal, _default);
    if( equal.kind != Common ) CARLA_RETURN_DEFAULT;
    tk = std::get<Token>(equal.content);

    carla::Decl d(identifier, carla::Type(str, type));
    switch (tk.kind) {
        case EQUAL: {
            d.k = carla::Decl::Hopefull;
            *result = d;
        } break;

        case SEMICOLON: {
            d.k = carla::Decl::Hopeless;
            *result = d;
        } break;

        default: CARLA_RETURN_DEFAULT;
    }

    (*index)--;
    return true;
}
