// Isto padroniza as mensagens de saida do compilador
#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

#define CARLA_DEBUG false

struct CompilerOutputs {
public:
    static void Log(std::string log);
    static void Fatal(std::string log);

    static void ClearCurrentLine();
};

// Common color macros
namespace Colorizer {
    constexpr const char* RESET = "\033[0m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* BOLD_RED = "\033[1;31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* BOLD_GREEN = "\033[1;32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BOLD_YELLOW = "\033[1;33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* BOLD_BLUE = "\033[1;34m";
    constexpr const char* PURPLE = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* BOLD_CYAN = "\033[1;36m";
    constexpr const char* DARK_YELLOW = "\033[33m";
    constexpr const char* DARK_RED = "\033[31m";
    constexpr const char* DARK_GREY = "\033[90m";
    constexpr const char* BOLD = "\033[1m";
}

void
CompilerOutputs::Log(std::string log)
{
    std::cout << "\r" << Colorizer::BOLD_GREEN << "[Carla]" << Colorizer::RESET << ": " << log;
}

void
CompilerOutputs::Fatal(std::string log)
{
    std::cout << "\r" << Colorizer::BOLD_RED << "[Carla FAIL]" << Colorizer::RESET << ": " << log << std::endl;
    std::exit(-1);
}


void
CompilerOutputs::ClearCurrentLine() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD consoleWidth = csbi.dwSize.X;
    COORD cursorPos = csbi.dwCursorPosition;
    DWORD charsWritten;
    cursorPos.X = 0;
    SetConsoleCursorPosition(hConsole, cursorPos);
    FillConsoleOutputCharacter(hConsole, ' ', consoleWidth, cursorPos, &charsWritten);
    SetConsoleCursorPosition(hConsole, cursorPos);
#else
    std::cout << "\033[2K\033[0G" << std::flush;
#endif
}
