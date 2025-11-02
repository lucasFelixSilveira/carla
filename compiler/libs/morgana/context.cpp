#include "context.hpp"

std::string Context::string() { return ss.str(); }

Context& Context::operator<<(const std::string& str) {
    ss << str;
    return *this;
}
