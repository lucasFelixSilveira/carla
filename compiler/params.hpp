// Isto armazena os dados do compilador
#pragma once

#include "compiler_outputs.hpp"
#include "libs/eva.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

typedef struct CompilerParams {
public:
    std::string cwd;
    std::string command;
    std::string main;
    bool optimized;
    std::string target;
    bool verbose;

    bool ffi;
    std::string c_path;

    CompilerParams(std::string cwd, std::string command, std::string main, bool optimized, std::string target, bool verbose)
        : cwd(cwd),
          command(command),
          main(main),
          optimized(optimized),
          target(target),
          verbose(verbose) {};

    static struct CompilerParams format(int argc, char **argv) {
        char *cwd  = argv[0];
        char *command = argv[1];
        char *main = (char*) "main.crl";
        char *target = (char*) "unknown";
        bool optimized = false;
        bool verbose = false;

        if( std::string(command) == "build" || std::string(command) == "run" ) {
            eva reader("target.eva");
            try { auto m = reader.get<std::string>("target", "main");
                  main = (char*) m.c_str();
            } catch(std::runtime_error e) {}
        }

        int i = 2;
        for(; i < argc; i++ ) {
            char *arg = argv[i];
            if( std::strcmp(argv[i], "-m") == 0 && (i + 1) < argc ) main = argv[++i];
            if( std::strcmp(argv[i], "-o") == 0 && (i + 1) < argc ) target = argv[++i];
            if( std::strcmp(argv[i], "-O") == 0 ) optimized = true;
            if( std::strcmp(argv[i], "-v") == 0 ) verbose = true;
        }

        return CompilerParams(cwd, command, main, optimized, target, verbose);
    }
} CompilerParams;
