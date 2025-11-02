#pragma once
#include <sstream>

struct Context {
    std::stringstream ss;

    Context() = default;

    Context& operator<<(const std::string& str);

    std::string string();
};
