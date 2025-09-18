// Isto padroniza as mensagens de saida do compilador
#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#define DEBUG true

typedef struct CompilerOutputs {
public:
    static void Log(std::string log);
    static void Fatal(std::string log);
} CompilerOutputs;

// Common color macros
#define BOLD_GREEN "\033[1;32m"
#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"

void
CompilerOutputs::Log(std::string log)
{
    std::cout << "\r" << BOLD_GREEN << "[Carla]" << RESET << ": " << log;
}

void
CompilerOutputs::Fatal(std::string log)
{
    std::cout << "\r" << BOLD_RED << "[Carla FAIL]" << RESET << ": " << log << std::endl;
    std::exit(-1);
}
