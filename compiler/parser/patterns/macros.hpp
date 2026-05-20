#pragma once

#include "../pattern.hpp"

#define NEED_BLOCK(t) *result = carla::t((*ctx)[++(*index)])

bool macros(CARLA_PATTERN_ARGUMENTS, size_t macro) {
    CARLA_PATTERN_STARTS(bool, false);
    switch(macro) {
        case START: *result = carla::Start(); break;
        default: CARLA_RETURN_DEFAULT;
    }
    (*index)++;
    return true;
}
