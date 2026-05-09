#pragma once

#include <string>
#include <vector>

#include "expression.hpp"

namespace carla {
    struct Call {
        std::string func;
        std::vector<carla::Expr> args;
        carla::Type result;

        Call(std::string func, std::vector<carla::Expr> args, carla::Type result)
            : func(func),
              args(args),
              result(result) {}
    };
}
