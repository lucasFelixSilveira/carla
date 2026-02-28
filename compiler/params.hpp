// Isto armazena os dados do compilador
#pragma once

#include "compiler_outputs.hpp"
#include "toml/reader.hpp"
#include <cstring>
#include <string>
#include <variant>

typedef struct CompilerParams {
public:
    std::string cwd;
    std::string command;
    std::string main;
    bool optimized;
    std::string target;

    CompilerParams(std::string cwd, std::string command, std::string main, bool optimized, std::string target) : cwd(cwd), command(command), main(main), optimized(optimized), target(target) {};

    static struct CompilerParams format(int argc, char **argv) {
        char *cwd  = argv[0];
        char *command = argv[1];
        char *main = (char*) "main.crl";
        char *target = (char*) "unknown";
        bool optimized = false;

        TOMLReader reader("carla", "target.toml");
        auto m = reader.get(TOMLReader::Expr("target", "main"));
        if( std::holds_alternative<TOMLReader::Error>(m) ) {
            auto err = std::get<TOMLReader::Error>(m);
            CompilerOutputs::Warn("Error in target.toml: " + std::get<0>(err) + "\n");
        } else {
            main = reader.check<std::string>("main", m).data();
        }

        int i = 2;
        for(; i < argc; i++ ) {
            char *arg = argv[i];
            if( std::strcmp(argv[i], "-m") == 0 && (i + 1) < argc ) main = argv[++i];
            if( std::strcmp(argv[i], "-o") == 0 && (i + 1) < argc ) target = argv[++i];
            if( std::strcmp(argv[i], "-O") == 0 ) optimized = true;
        }

        return CompilerParams(cwd, command, main, optimized, target);
    }
} CompilerParams;
