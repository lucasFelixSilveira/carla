// Isto armazena os dados do compilador
#pragma once

#include <cstring>
#include <string>

typedef struct CompilerParams {
public:
    std::string cwd;
    std::string command;
    std::string main;
    bool optimized;

    static struct CompilerParams build(std::string cwd, std::string command, std::string main, bool optimized);
    static struct CompilerParams format(int argc, char **argv);
} CompilerParams;

CompilerParams
CompilerParams::build(std::string cwd, std::string command, std::string main, bool optimized)
{
    return (CompilerParams) { cwd, command, main, optimized };
}

CompilerParams
CompilerParams::format(int argc, char **argv)
{
    char *cwd  = argv[0];
    char *command = argv[1];
    char *main = (char*) "main.crl";
    bool optimized = false;

    int i = 2;
    for(; i < argc; i++ ) {
        char *arg = argv[i];
        if( std::strcmp(argv[i], "-m") == 0 && (i + 1) < argc ) main = argv[++i];
        if( std::strcmp(argv[i], "-o") == 0 ) optimized = true;
    }

    return CompilerParams::build(cwd, command, main, optimized);
}
