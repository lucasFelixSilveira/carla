#include "storage.hpp"
#include <sstream>

std::string Storage::string() {
    std::stringstream ss;
    for(auto& alias : aliases) {
        ss << "alias a" << std::get<0>(alias) << " = " << std::get<1>(alias) << '\n';
    }
    return ss.str();
}

void Storage::leave() {
    local = 0;
}
