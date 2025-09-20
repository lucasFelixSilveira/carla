#include "compiler_outputs.hpp"
#include "params.hpp"
#include "tokenizer/scanner.hpp"
#include "tokenizer/token.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include "parser/parser.hpp"
#include "globals.hpp"

int
main(int argc, char **argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    CompilerOutputs::Log("Starting the compilation!");

    int min_arguments = 2;
    if( argc < min_arguments ) CompilerOutputs::Fatal("You need enter with a action. If you don't know the acceptable actions, use: help.");

    GlobalArgs::instance().set(argc, argv);
    CompilerParams params = CompilerParams::format(argc, argv);

    /* checa se o arquivo esta acessivel */
    std::ifstream file(params.main, std::ios::binary | std::ios::ate);
    if(! file.is_open() ) CompilerOutputs::Fatal("Your main file is not valid. Try use -m to define the newest file");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> src(size);

    if(! file.read(src.data(), size) ) CompilerOutputs::Fatal("Your main file is not valid. Try use -m to define the newest file");

    /* Lexical phase */
    std::vector<Token> tokens = Scanner::read(src, size);

    /* Parser phase */
    Parser::parse(tokens);

    /* Calculo do tempo que demorou para a compilacao */
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

    float seconds = ((double) ms.count()) / 1000.0;
    char *duration = (char*) std::malloc(32);
    std::sprintf(duration, "Total compilation time: %s%.2fs%s\x0a", BOLD_GREEN, seconds, RESET);
    CompilerOutputs::Log((std::string) duration);
    free(duration);

    /* DEBUG: */
    // dvecprint(tokens);

    int success_code = 0;
    return success_code;
}
