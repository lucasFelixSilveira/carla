#pragma once

#include "morgana/context.hpp"
#include "morgana/types.hpp"
#include <iostream>
#include <sstream>
#include <string>

#include "./morgana/storage.hpp"
#include "./morgana/context.hpp"
#include "./morgana/types.hpp"

#define morgana_func(id, args, block) \
    std::string id args {             \
        std::stringstream ss;         \
        do block while(0);            \
        return ss.str();              \
    }

namespace morgana {
    size_t last(
        Storage *storage,
        std::string what
    ) { return storage->last.at(what); };

    morgana_func(alloc, (
        Storage *storage,
        morgana::type type
    ), {
        size_t i = storage->variable.top();
        storage->last.insert({ "alloc", i });
        size_t added = 1;
        ss << "_" << i << " = alloc " << type_string(type) << '\n';
        storage->variable.pop();
        storage->variable.push(i + added);
    });

    morgana_func(store, (
        size_t x, size_t y
    ), {
        ss << "store _" << x << " _" << y << '\n';
    });

    morgana_func(static_declaration, (
        Storage *storage,
        std::string buff
    ), {
        size_t i = storage->variable.top();
        storage->last.insert({ "expr", i });
        size_t added = 1;
        ss << "_" << i << " = constant \"" << buff << "\"\n";
        storage->variable.pop();
        storage->variable.push(i + added);
    });

    morgana_func(function, (
        Storage *storage,
        std::string name,
        morgana::type ret,
        std::vector<morgana::type> args,
        Context& context
    ), {
        size_t i = storage->variable.top();
        size_t added = 1;
        ss << '\n' << type_string(ret) << " " << name << "(";
        for( size_t i = 0; i < args.size(); i++ ) {
            ss << type_string(args[i]);
            if( i < args.size() - 1 ) ss << ", ";
        }
        ss << ") {\n" << context.string() << "}\n";
        storage->variable.pop();
        storage->variable.push(i + added);
    });

    morgana_func(comptime, (
        std::string identifier
    ), {
        ss << "comptime " << identifier << "\n";
    });

    morgana_func(puts, (
        Storage *storage,
        std::string string
    ), {
        size_t i = storage->variable.top();
        size_t added = 1;

        ss << "_" << i << " = constant \"" << string << "\"\n"
           << "puts _" << i << "\n";

        storage->variable.pop();
        storage->variable.push(i + added);
    });
}
