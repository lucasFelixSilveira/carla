#pragma once

#include <optional>
#include <sstream>
#include <tuple>
#include <variant>
#include <vector>

#include "../symbols.hpp"
#include "../pattern.hpp"

#include "../../libs/morgana/types.hpp"

std::tuple<bool, morgana::type, std::string>
typement(CARLA_PATTERN_ARGUMENTS) {
    auto d = std::make_tuple(false, std::monostate(), std::string());
    CARLA_PATTERN_STARTS(auto, d);
    CARLA_GET_NEXT(first, _default);

    std::stringstream ss;

    if( first.kind == Block ) {
        ss << '[';
        std::vector<morgana::type> t;
        auto body = std::get<std::vector<pContext>>(first.content);

        size_t i = 0;
        while(i < body.size()) {
            auto [success, s, str] = typement(result, sym, &i, &body);
            if(! success ) CARLA_RETURN_DEFAULT;
            t.push_back(s);
            ss << str;
            if( (i + 1) < body.size() ) {
                CARLA_INDEX_NEXT(comma, _default, (&body), (&i));
                if( comma.kind != Common ) CARLA_RETURN_DEFAULT;

                auto tk = std::get<Token>(comma.content);
                if( tk.kind != COMMA ) CARLA_RETURN_DEFAULT;
                ss << ", ";
            }
        }

        ss << ']';
        bool is_ptr = false;
        int levels = 0;
        CARLA_PEEK_NEXT(ptr, _default);
        if( ptr.kind == Common ) {
            auto tk = std::get<Token>(ptr.content);

            if( tk.kind != STAR ) {
                return { true, morgana::tuple(t), ss.str() };
            }

            is_ptr = true;
            while(1) {
                CARLA_GET_NEXT(ptr, _default);
                if( ptr.kind != Common ) break;
                tk = std::get<Token>(ptr.content);
                if( tk.kind != STAR ) break;
                levels++;
            }
        }

        ss << std::string(levels, '*');
        if( is_ptr ) return { true, morgana::ptr(), ss.str() };
        return { true, morgana::tuple(t), ss.str() };
    }

    auto tk = std::get<Token>(first.content);
    if( tk.kind != IDENTIFIER ) CARLA_RETURN_DEFAULT;

    std::string type_str = tk.lexeme;
    auto type = sym->findSymbol(tk.lexeme);
    if( type == nullptr ) CARLA_RETURN_DEFAULT;
    if(! std::holds_alternative<morgana::type>(*type) ) CARLA_RETURN_DEFAULT;
    std::optional<morgana::type> t;

    bool is_ptr = false;
    int levels = 0;
    if( ((*index) + 1) >= ctx->size() ) {
        t.emplace(std::get<morgana::type>(*type));
    }
    else {
        CARLA_PEEK_NEXT(ptr, _default);
        if( ptr.kind == Common ) {
            tk = std::get<Token>(ptr.content);

            if( tk.kind != STAR ) {
                return { true, std::get<morgana::type>(*type), type_str };
            }

            is_ptr = true;
            while(1) {
                CARLA_GET_NEXT(ptr, _default);
                if( ptr.kind != Common ) break;
                tk = std::get<Token>(ptr.content);
                if( tk.kind != STAR ) break;
                levels++;
            }
        }
    }

    if( is_ptr ) { t.emplace(morgana::ptr()); }
    else { t.emplace(std::get<morgana::type>(*type)); }

    return { true, t.value(), type_str + std::string(levels, '*') };
}
