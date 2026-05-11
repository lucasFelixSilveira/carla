#pragma once

#include <sstream>
#include <string>

struct Builder {
    std::stringstream ss;
    std::string string() const { return ss.str(); }
    void operator<<(std::string str) { ss << str; }
    Builder() : ss() {}
    ~Builder() {}
};
