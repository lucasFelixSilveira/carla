#pragma once

#include "./parser/symbols.hpp"
#include "libs/morgana.hpp"
#include <memory>

void charset(Symt& sym) {
    auto i8 = morgana::type::integer(8).shared();
    auto i16 = morgana::type::integer(16).shared();
    auto i32 = morgana::type::integer(32).shared();
    auto i64 = morgana::type::integer(64).shared();
    auto usize = morgana::type::integer(sizeof(char*) * 8).shared();

    sym.entry();
    sym.addSymbol("int", usize);
    sym.addSymbol("int8", i8);
    sym.addSymbol("int16", i16);
    sym.addSymbol("int32", i32);
    sym.addSymbol("int64", i64);

    auto dyn = std::make_shared<special>(special("dyn"));
    sym.addSymbol("dyn", dyn);

    auto stackptr = std::make_shared<special>(special("stackptr", 2, {"dyn", "int"}));
    sym.addSymbol("stackptr", stackptr);
}
