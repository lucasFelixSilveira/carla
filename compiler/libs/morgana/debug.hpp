#pragma once

#include <memory>
#include <variant>
#include <iostream>

#include "../morgana.hpp"
#include "builder.hpp"
#include "builder.hpp"

struct Debug {
public:
    using add_t = std::variant<std::shared_ptr<morgana::type>>;
    static void add(Builder& builder, add_t that) {
        if( std::holds_alternative<std::shared_ptr<morgana::type>>(that) ) {
            Debug::print(builder, "Has been added to stack: " + std::get<std::shared_ptr<morgana::type>>(that)->string());
        }
    }

    static void print(Builder& builder, const std::string& message) {
        if( builder.isDebugMode() ) {
            std::cout << "[Morgana IR]: " << message << std::endl;
        }
    }
};
