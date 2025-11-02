#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../libs/morgana.hpp"

using Symbol = std::variant<
    std::shared_ptr<morgana::type>,
    std::shared_ptr<morgana::variable>
>;

using Symbols = std::tuple<int, std::vector<std::unordered_map<std::string, Symbol>>>;

struct Symt {
public:
    void entry();
    void exit();
    void dump();

    void addSymbol(const std::string& name, const Symbol& symbol);
    void removeSymbol(const std::string& name);
    Symbol* findSymbol(const std::string& name);

private:
    Symbols symbols = {};
};

void Symt::entry() {
    std::get<0>(symbols)++;
    std::get<1>(symbols).push_back({});
}

void Symt::exit() {
    std::get<0>(symbols)--;
    std::get<1>(symbols).pop_back();
}

void Symt::addSymbol(const std::string& name, const Symbol& symbol) {
    std::get<1>(symbols).back()[name] = symbol;
}

void Symt::removeSymbol(const std::string& name) {
    std::get<1>(symbols).back().erase(name);
}

Symbol* Symt::findSymbol(const std::string& name) {
    for( auto it = std::get<1>(symbols).rbegin(); it != std::get<1>(symbols).rend(); ++it ) {
        auto symbol = it->find(name);
        if( symbol != it->end() ) {
            return &symbol->second;
        }
    }
    return nullptr;
}
