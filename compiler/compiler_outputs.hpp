// Isto padroniza as mensagens de saida do compilador
#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#define CARLA_DEBUG true

struct CompilerOutputs {
public:
    static void Log(std::string log);
    static void Fatal(std::string log);
};

// Common color macros
#define RESET "\033[0m"
#define RED "\033[31m"
#define BOLD_RED "\033[1;31m"
#define GREEN "\033[32m"
#define BOLD_GREEN "\033[1;32m"
#define YELLOW "\033[33m"
#define BOLD_YELLOW "\033[1;33m"
#define BLUE "\033[34m"
#define BOLD_BLUE "\033[1;34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define DARK_YELLOW "\033[33m"
#define DARK_RED "\033[31m"
#define DARK_GREY "\033[90m"
#define BOLD "\033[1m"

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
