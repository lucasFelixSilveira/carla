#pragma once

#include "./parser/symbols.hpp"
#include "libs/morgana.hpp"

void charset(Symt& sym) {
    auto i8 = morgana::type::integer(8).shared();
    auto i16 = morgana::type::integer(16).shared();
    auto i32 = morgana::type::integer(32).shared();
    auto i64 = morgana::type::integer(64).shared();

    sym.entry();
    sym.addSymbol("int8", i8);
    sym.addSymbol("int16", i16);
    sym.addSymbol("int32", i32);
    sym.addSymbol("int64", i64);
}
