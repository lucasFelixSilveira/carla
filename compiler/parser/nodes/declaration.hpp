#pragma once

#include <string>
#include "type.hpp"

namespace carla {
    struct Decl {
        enum kind { Hopeless, Hopefull, HopefullNontyped };
        std::string identiifer;
        carla::Type type;
        kind k;

        Decl(std::string identifier, carla::Type type)
            : identiifer(identifier),
              type(type) {}
    };
}
