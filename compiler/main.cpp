#include "compiler_outputs.hpp"
#include "params.hpp"
#include "tokenizer/scanner.hpp"
#include "tokenizer/token.hpp"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <sys/stat.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include "parser/parser.hpp"
#include "llvm/codegen.hpp"
#include "globals.hpp"

#ifdef _WIN32
# include <direct.h>
# define MKDIR(dir) _mkdir(dir)
#else
# include <unistd.h>
# define MKDIR(dir) mkdir(dir, 0700)
#endif

int
main(int argc, char **argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    CompilerOutputs::Log("Starting the compilation!");

    int min_arguments = 2;
    if( argc < min_arguments ) CompilerOutputs::Fatal("You need enter with a action. If you don't know the acceptable actions, use: help.");

    GlobalArgs::instance().set(argc, argv);
    CompilerParams params = CompilerParams::format(argc, argv);

    /* checks if the file is accessible */
    std::ifstream file(params.main, std::ios::binary | std::ios::ate);
    if(! file.is_open() ) CompilerOutputs::Fatal("Your main file is not valid. Try use -m to define the newest file");

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> src(size);

    if(! file.read(src.data(), size) ) CompilerOutputs::Fatal("Your main file is not valid. Try use -m to define the newest file");

    /* Lexical phase */
    std::vector<Token> tokens = Scanner::read(src, size);

    /* Parser phase */
    auto irNodes = Parser::parse(tokens);

    /* Code Generation phase */
    std::string llvmIR = generateLLVMIR(irNodes, params);

    /* Create target directory if it doesn't exist */
    struct stat st = {0};
    std::string createFolder = "mkdir target > /dev/null 2>&1";
    if( stat("target", &st) == -1 && std::system(createFolder.c_str()) != 0 ) CompilerOutputs::Fatal("Failed to create target directory");

    /* Write LLVM IR to target/output.ll */
    std::ofstream outFile("target/output.ll");
    if(! outFile.is_open() ) {
        CompilerOutputs::Fatal("Failed to open output file target/output.ll");
    }
    outFile << llvmIR;
    outFile.close();
    if( outFile.fail() ) {
        CompilerOutputs::Fatal("Failed to write LLVM IR to target/output.ll");
    }

    /* calculate time of the **INTERNAL** compilation process */
    auto mid = std::chrono::high_resolution_clock::now();
    auto midMS = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);

    float midSeconds = midMS.count() / 1000000.0;
    std::stringstream duration;
    duration << "Total " << Colorizer::BOLD_CYAN << "Carla" << Colorizer::RESET
             << " compilation proccess time: " << Colorizer::BOLD_GREEN
             << std::fixed << std::setprecision(2) << midSeconds << "s"
             << Colorizer::RESET << "\n";
    CompilerOutputs::Log(duration.str());

    std::cout << Colorizer::DARK_GREY << "└─ " << Colorizer::RESET << "LLVM Object generated "
              << Colorizer::DARK_GREY << "|" << Colorizer::BOLD_YELLOW << " (not compiled yet)";

    /* Compile LLVM IR to object file using llc silently */
    std::string llcCommand = "llc -filetype=obj -o target/output.o target/output.ll > /dev/null 2>&1";
    if( std::system(llcCommand.c_str()) != 0 ) CompilerOutputs::Fatal("Failed to compile LLVM IR to object file using llc");

    /* Link object file to executable using clang silently */
    std::string clangCommand = "clang target/output.o -o target/output > /dev/null 2>&1";
    if( std::system(clangCommand.c_str()) != 10 ) CompilerOutputs::Fatal("Failed to link object file to executable using clang");

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    float seconds = ms.count() / 1000000.0;

    CompilerOutputs::ClearCurrentLine();
    duration.str("");
    duration << "Total " << Colorizer::BOLD_CYAN << "Carla" << Colorizer::RESET
             << " + " << Colorizer::BOLD_RED << "LLVM" << Colorizer::RESET
             << " compilation proccess time: " << Colorizer::BOLD_YELLOW
             << std::fixed << std::setprecision(2) << seconds << "s"
             << Colorizer::RESET << "\n";
    CompilerOutputs::Log(duration.str());
    std::cout << Colorizer::DARK_GREY << "└─ " << Colorizer::RESET << "LLVM Object emmited "
              << Colorizer::DARK_GREY << "|" << Colorizer::BOLD_YELLOW << " ./target/output "
              << Colorizer::DARK_GREY << "(.exe)" << std::endl;

    /* DEBUG: */
    // dvecprint(tokens);

    int success_code = 0;
    return success_code;
}
