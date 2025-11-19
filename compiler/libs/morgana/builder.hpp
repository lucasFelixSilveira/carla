#pragma once
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "../morgana.hpp"

struct Builder;

struct MorganaSymbol {
public:
    using value = std::variant<std::monostate, std::shared_ptr<morgana::type>>;

    std::string name;
    value type;

    MorganaSymbol(std::string name, value type) : name(name), type(type) {}
};

struct MorganaSymbols {
private:
    std::vector<MorganaSymbol> symbols;

public:
    MorganaSymbols() = default;
    void add(Builder& builder, const std::string& name, std::shared_ptr<morgana::type> type);
};

/*
 * This class is the base class for building the IR.
 *
 * How it works?
 * - That is the "Storage" class. The Storage class is responsible for storing the IR and
 *   their miscellaneous information.
 */
class Builder {
private:
    std::stringstream ss;
    bool debug = false;

public:
    long addr = 0;
    MorganaSymbols symbols;

    std::string string() const;
    bool isDebugMode() const;
    Builder& operator<<(const std::string& str);

    Builder(bool debug = false) : debug(debug) {
        symbols = MorganaSymbols();
    }
};
