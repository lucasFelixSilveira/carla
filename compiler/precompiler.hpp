#pragma once

#include "compiler_outputs.hpp"
#include "params.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/scanner.hpp"
#include "tokenizer/token_kind.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<Token> precompiler(std::vector<char> source, std::streamsize size, CompilerParams& param) {
    std::vector<Token> scanner = Scanner::read(source, size);
    std::vector<std::string> already_imported_files;

    while(true) {
        bool changed = false;
        for( int i = 0; i < scanner.size(); i++ ) {
            switch(scanner[i].kind) {
                case IMPORT: {
                    size_t start = i;
                    auto err = []() {
                        CompilerOutputs::Fatal("Import comptime statement needs look like `import(\"file\")`");
                    };

                    if( scanner[++i].kind != LEFT_PAREN ) err();
                    if( scanner[i + 1].kind != STRING ) err();

                    std::string data = scanner[++i].lexeme;

                    if( scanner[++i].kind != RIGHT_PAREN ) err();
                    if( scanner[++i].kind != SEMICOLON ) err();

                    auto basename = data.substr(1, data.size() - 2);

                    auto path = std::filesystem::absolute(
                        std::filesystem::path(param.main).parent_path() / basename
                    );

                    auto it = std::find(already_imported_files.begin(), already_imported_files.end(), path);
                    if( it != already_imported_files.end() ) {
                        scanner.erase(scanner.begin() + start, scanner.begin() + i + 1);
                        continue;
                    };

                    already_imported_files.push_back(path);
                    std::ifstream file(path, std::ios::binary | std::ios::ate);
                    if(! file.is_open() )CompilerOutputs::Fatal("Your " + basename + " file is not valid.");

                    std::streamsize size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    std::vector<char> src(size);

                    if(! file.read(src.data(), size) ) CompilerOutputs::Fatal("Your " + basename + " file is not valid.");

                    std::vector<Token> tks = Scanner::read(src, size);

                    scanner.erase(scanner.begin() + start, scanner.begin() + i + 1);
                    scanner.insert(scanner.begin() + start, tks.begin(), tks.end());

                    i = start + tks.size() - 1;
                    changed = true;
                } break;
                default: {};
            }
        }

        if(! changed ) return scanner;
    }
}
