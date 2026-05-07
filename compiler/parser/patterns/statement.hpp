#pragma once

#include "../pattern.hpp"
#include "../nodes/statement.hpp"

bool statement(CARLA_PATTERN_ARGUMENTS, const std::string data) {
    CARLA_PATTERN_STARTS(bool, false);
    (*index)++;
    result->~pNode();
    new(result) pNode(carla::Stmt(data));
    return true;
}
