#pragma once

#include "params.hpp"
#include "parser/symbols.hpp"
#include "libs/eva.hpp"

void load_ffi(Symt& symbols, CompilerParams& params) {
    eva reader("target.eva");

    try { auto enabled = reader.get<bool>("ffi", "enabled");
         params.ffi = enabled;
         if(! enabled ) return;
    } catch(std::runtime_error e) {
        params.ffi = false;
        return;
    }

}
