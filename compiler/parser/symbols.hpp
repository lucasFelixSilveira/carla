// Isto representa a tabela de simbolos do compilador

#pragma once

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include "../compiler_outputs.hpp"

#include "llvm_rtypes.hpp"

enum SymKind {
    VARIABLE,
    TYPE,
    FUNCTION
};

enum SymVariant {
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_PRIMITIVE,
    CUSTOM_INTEGER,
    CUSTOM_UINTEGER,
    VAR_LOCAL,
    VAR_GLOBAL,
    VAR_PARAM
};

struct Symbol {
public:
    SymKind kind;
    SymVariant variant;
    int bytes = 0;
    int scopeLevel = 0;

    LLVMRadicalType radical;

    Symbol(SymKind kind, SymVariant variant, LLVMRadicalType radical, int bytes, int scopeLevel = 0)
        : kind(kind), variant(variant), radical(radical), bytes(bytes), scopeLevel(scopeLevel) {};

    Symbol(SymKind kind, Symbol type) {
        this->kind = kind;
        this->variant = type.variant;
        this->radical = type.radical;
        this->bytes = type.bytes;
        this->scopeLevel = type.scopeLevel;
    };

    Symbol() = default;
};

struct Symbols {
private:
    int currentScope = 0;
    std::map<std::string, Symbol> symbols;

public:
    Symbols() {
        int sys = sizeof(int);
        LLVMRadicalType sysl = (LLVMRadicalType) sys;

        symbols.insert({"void",   Symbol(TYPE, TYPE_PRIMITIVE,  void_t, 0,    0)});

        symbols.insert({"int",    Symbol(TYPE, TYPE_PRIMITIVE,  sysl,   sys,  0)});
        symbols.insert({"uint",   Symbol(TYPE, TYPE_PRIMITIVE,  sysl,   sys,  0)});

        symbols.insert({"int8",   Symbol(TYPE, TYPE_PRIMITIVE,  i8,     1,    0)});
        symbols.insert({"int16",  Symbol(TYPE, TYPE_PRIMITIVE,  i16,    2,    0)});
        symbols.insert({"int32",  Symbol(TYPE, TYPE_PRIMITIVE,  i32,    4,    0)});
        symbols.insert({"int64",  Symbol(TYPE, TYPE_PRIMITIVE,  i64,    8,    0)});
        symbols.insert({"int128", Symbol(TYPE, TYPE_PRIMITIVE,  i128,   16,   0)});
        symbols.insert({"int256", Symbol(TYPE, TYPE_PRIMITIVE,  i256,   32,   0)});

        symbols.insert({"uint8",   Symbol(TYPE, TYPE_PRIMITIVE, i8,     1,    0)});
        symbols.insert({"uint16",  Symbol(TYPE, TYPE_PRIMITIVE, i16,    2,    0)});
        symbols.insert({"uint32",  Symbol(TYPE, TYPE_PRIMITIVE, i32,    4,    0)});
        symbols.insert({"uint64",  Symbol(TYPE, TYPE_PRIMITIVE, i64,    8,    0)});
        symbols.insert({"uint128", Symbol(TYPE, TYPE_PRIMITIVE, i128,   16,   0)});
        symbols.insert({"uint256", Symbol(TYPE, TYPE_PRIMITIVE, i256,   32,   0)});
    }

    // Entra em um novo escopo
    void enterScope() {
        currentScope++;
    }

    // Sai do escopo atual e remove símbolos desse escopo
    void exitScope() {
        if (currentScope > 0) {
            remove([](const Symbol& sym, int currentScope) {
                return sym.scopeLevel == currentScope;
            });
            currentScope--;
        }
    }

    // Obtém o escopo atual
    int getCurrentScope() const {
        return currentScope;
    }

    Symbol get(const std::string& id) {
        auto it = symbols.find(id);
        if (it == symbols.end()) {
            CompilerOutputs::Fatal("Símbolo não encontrado!");
        }
        return it->second;
    }

    // Verifica se símbolo existe
    bool contains(const std::string& id) const {
        return symbols.find(id) != symbols.end();
    }

    // Adiciona símbolo no escopo atual - CORRIGIDO: usa insert ou emplace
    void add(const std::string& id, const Symbol& symbol) {
        Symbol symWithScope = symbol;
        symWithScope.scopeLevel = currentScope;

        auto result = symbols.insert({id, symWithScope});
        if (!result.second) {
            result.first->second = symWithScope;
        }
    }

    // Adiciona símbolo com escopo específico - CORRIGIDO
    void add(const std::string& id, SymKind kind, SymVariant variant, LLVMRadicalType radical, int bytes) {
        Symbol newSymbol(kind, variant, radical, bytes, currentScope);
        auto result = symbols.insert({id, newSymbol});
        if (!result.second) {
            result.first->second = newSymbol;
        }
    }

    // Remove símbolos baseado em um filtro de escopo
    void remove(std::function<bool(const Symbol&, int)> filter) {
        std::vector<std::string> toRemove;

        // Primeiro identifica quais símbolos remover
        for (const auto& pair : symbols) {
            if (filter(pair.second, currentScope)) {
                toRemove.push_back(pair.first);
            }
        }

        // Depois remove os identificados
        for (const auto& key : toRemove) {
            symbols.erase(key);
        }
    }

    // Remove todos os símbolos de um escopo específico
    void removeByScope(int scopeLevel) {
        remove([scopeLevel](const Symbol& sym, int) {
            return sym.scopeLevel == scopeLevel;
        });
    }

    // Remove símbolos do escopo atual
    void removeCurrentScope() {
        removeByScope(currentScope);
    }

    // Remove símbolos com escopo maior que o atual (útil para cleanup)
    void removeScopesGreaterThanCurrent() {
        remove([](const Symbol& sym, int currentScope) {
            return sym.scopeLevel > currentScope;
        });
    }

    // Lista todos os símbolos (útil para debugging)
    void listAll() const {
        for (const auto& pair : symbols) {
            std::cout << pair.first << " -> escopo: " << pair.second.scopeLevel
                      << ", tipo: " << pair.second.kind
                      << ", bytes: " << pair.second.bytes << std::endl;
        }
    }

    // Limpa todos os símbolos (exceto os globais)
    void clearNonGlobal() {
        remove([](const Symbol& sym, int) {
            return sym.scopeLevel > 0;
        });
    }
};
