#pragma once

#include "../pattern.hpp"

bool macros(CARLA_PATTERN_ARGUMENTS, size_t macro) {
    CARLA_PATTERN_STARTS(bool, false);
    switch(macro) {
        case START: *result = carla::Start(); break;
        default: CARLA_RETURN_DEFAULT;
    }
    (*index)++;
    return true;
}
