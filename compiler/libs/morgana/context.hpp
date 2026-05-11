#pragma once

#include <sstream>
#include <string>

struct Context {
    std::stringstream ss;
    std::string string() const { return ss.str(); }
    void operator<<(std::string str) { ss << str; }
    Context() : ss() {}
    ~Context() = default;
};
