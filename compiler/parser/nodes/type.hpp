#pragma once

#include <string>
#include "../../libs/morgana/types.hpp"

namespace carla {
    struct Type {
        std::string carla;
        morgana::type morgana;

        Type(std::string carla, morgana::type morgana)
            : carla(carla),
              morgana(morgana) {}
    };
};
