#pragma once

#include "libs/morgana.hpp"
#include "params.hpp"
#include "parser/symbols.hpp"
#include "toml/reader.hpp"
void load_ffi(Symt& symbols, CompilerParams& params) {
    TOMLReader reader("carla", "target.toml");
    auto ffi = reader.get(TOMLReader::Expr("ffi", "enabled"));
    if( std::holds_alternative<TOMLReader::Error>(ffi) ) {
        auto err = std::get<TOMLReader::Error>(ffi);
        CompilerOutputs::Warn("Error in target.toml: " + std::get<0>(err) + "\n");
    } else {
        bool ffi_enabled = reader.check<bool>("enabled", ffi);
        if(! ffi_enabled ) {
            params.ffi = false;
            return;
        }

        std::string ffi_path;
        std::vector<std::string> ffi_exports;

        auto path = reader.get(TOMLReader::Expr("ffi", "path"));
        if( std::holds_alternative<TOMLReader::Error>(path) ) {
            auto err = std::get<TOMLReader::Error>(path);
            CompilerOutputs::Warn("Error in target.toml: " + std::get<0>(err) + "\n");
        } else {
            ffi_path = reader.check<std::string>("path", path);
        }

        auto exports = reader.get(TOMLReader::Expr("ffi", "export"));
        if( std::holds_alternative<TOMLReader::Error>(exports) ) {
            auto err = std::get<TOMLReader::Error>(exports);
            CompilerOutputs::Warn("Error in target.toml: " + std::get<0>(err) + "\n");
        } else {
            ffi_exports = reader.check<std::vector<std::string>>("export", exports);
        }

        for( std::string identifier : ffi_exports ) {
            symbols.addSymbol(identifier, morgana::ffi_callable(ffi_path));
        }

        params.ffi = true;
        params.c_path = ffi_path;
    }
}
