#pragma once

#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace morgana {
    struct integer;
    struct ptr;
    struct void_t;
    struct tuple;

    using type = std::variant<std::monostate, integer, ptr, void_t, tuple>;

    struct integer {
        int bits;
        integer(int bits) : bits(bits) {}
        ~integer() = default;
    };

    struct ptr {
        ptr() {}
        ~ptr() = default;
    };

    struct void_t {
        void_t() {}
        ~void_t() = default;
    };

    struct tuple {
        std::vector<type> types;
        tuple(std::vector<type> types) : types(types) {}
        ~tuple() = default;
    };

    std::string type_string(type t) {
        if( std::holds_alternative<integer>(t) ) return "i" + std::to_string(std::get<integer>(t).bits);
        else if( std::holds_alternative<ptr>(t) ) return "ptr";
        else if( std::holds_alternative<void_t>(t) ) return "void";
        else if( std::holds_alternative<tuple>(t) ) {
            std::stringstream ss;
            ss << "tuple { ";
            for( const auto& type : std::get<tuple>(t).types )
            /* -> */ ss << type_string(type) << ",";
            ss << " }";
            return ss.str();
        }
        return "unknown";
    }
}
