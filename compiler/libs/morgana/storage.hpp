#pragma once

#include <stack>
#include <string>
#include <unordered_map>

struct Storage {
    std::stack<size_t> variable;
    std::unordered_map<std::string, size_t> last;
};
