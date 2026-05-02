#pragma once

#include "../pattern.hpp"

bool macros(CARLA_PATTERN_ARGUMENTS, size_t macro) {
    switch(macro) {
        case START: *result = carla::Start(); break;
        default: return false;
    }
    (*index)++;
    return true;
}
