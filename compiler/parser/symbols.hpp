#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>
#include <iomanip>

#include "../libs/morgana.hpp"

struct special;

using Symbol = std::variant<
    std::shared_ptr<special>,
    std::shared_ptr<morgana::type>,
    std::shared_ptr<morgana::variable>
>;

struct special {
    enum Kind { Comptime, Keyword };
    Kind kind;
    int size;
    std::vector<std::string> types;
    std::string name;
    special(std::string name, int size, std::vector<std::string> types) : kind(Kind::Comptime), size(size), types(types), name(name) {}
    special(std::string name) : kind(Kind::Keyword), size(0), types({}), name(name) {}
};

using Symbols = std::tuple<int, std::vector<std::unordered_map<std::string, Symbol>>>;

// === Classe Symt ===
struct Symt {
public:
    void entry();
    void exit();
    void dump() const;

    void addSymbol(const std::string& name, const Symbol& symbol);
    void removeSymbol(const std::string& name);
    Symbol* findSymbol(const std::string& name);

private:
    Symbols symbols = {};
};


inline void Symt::entry() {
    std::get<0>(symbols)++;
    std::get<1>(symbols).emplace_back();
}

inline void Symt::exit() {
    auto& scopes = std::get<1>(symbols);
    if (scopes.size() <= 1) return;
    std::get<0>(symbols)--;
    scopes.pop_back();
}

inline void Symt::addSymbol(const std::string& name, const Symbol& symbol) {
    if (std::get<1>(symbols).empty()) {
        std::get<1>(symbols).emplace_back();
    }
    std::get<1>(symbols).back()[name] = symbol;
}

inline void Symt::removeSymbol(const std::string& name) {
    auto& current = std::get<1>(symbols).back();
    current.erase(name);
}

inline Symbol* Symt::findSymbol(const std::string& name) {
    auto& scopes = std::get<1>(symbols);
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

inline void Symt::dump() const {
    const auto& scopes = std::get<1>(symbols);

    std::cout << "========================================\n";
    std::cout << " SYMBOL TABLE DUMP (" << scopes.size() << " scope(s))\n";
    std::cout << "========================================\n";

    for (size_t i = 0; i < scopes.size(); ++i) {
        const auto& scope = scopes[i];
        std::cout << "Scope #" << i << " (depth " << i << "):\n";

        if (scope.empty()) {
            std::cout << "  (empty)\n";
            continue;
        }

        for (const auto& [name, sym] : scope) {
            std::cout << "  " << std::setw(12) << std::left << name << " : ";
            if (std::holds_alternative<std::shared_ptr<morgana::type>>(sym)) {
                std::cout << "<type>";
            } else if (std::holds_alternative<std::shared_ptr<morgana::variable>>(sym)) {
                std::cout << "<variable>";
            } else {
                std::cout << "<unknown>";
            }
            std::cout << "\n";
        }
    }

    std::cout << "========================================\n";
}
