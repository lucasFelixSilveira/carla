#pragma once

#include <memory>
#include <tuple>
#include <vector>

struct Storage {
public:
    long long exprcount = 0;
    long long addr = 0;
    std::vector<std::tuple<long long, std::string>> aliases;
    long long local = 0;

    void leave();
    std::string string();
};
