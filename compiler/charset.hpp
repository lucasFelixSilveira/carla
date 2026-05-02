#pragma once

#include "./parser/symbols.hpp"
#include "libs/morgana.hpp"
#include <memory>

void charset(Symt& sym) {
    auto i8 = morgana::integer(8);
    auto i16 = morgana::integer(16);
    auto i32 = morgana::integer(32);
    auto i64 = morgana::integer(64);

    auto ptr = morgana::ptr();
    auto void_t = morgana::void_t();

    sym.entry();
    sym.addSymbol("int", morgana::type(i64));
    sym.addSymbol("int8", morgana::type(i8));
    sym.addSymbol("int16", morgana::type(i16));
    sym.addSymbol("int32", morgana::type(i32));
    sym.addSymbol("int64", morgana::type(i64));
    sym.addSymbol("string", morgana::type(ptr));

    sym.addSymbol("void", morgana::type(void_t));
}
