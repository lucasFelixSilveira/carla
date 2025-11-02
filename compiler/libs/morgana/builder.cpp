#include "builder.hpp"
#include "debug.hpp"

void MorganaSymbols::add(Builder& builder, const std::string& name, std::shared_ptr<morgana::type> type) {
    symbols.push_back(MorganaSymbol(name, type));
    Debug::add(builder, type);
}

std::string Builder::string() const { return ss.str(); }
bool Builder::isDebugMode() const { return debug; }

Builder& Builder::operator<<(const std::string& str) {
    ss << str;
    return *this;
}
