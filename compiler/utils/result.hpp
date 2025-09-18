// Isto simula um Option<T> do Rust, porem com o nome de Result<T>

#pragma once

#include <cstddef>
#include <string>
#include <variant>

struct Some {};
struct Err { std::string err; };

using Result = std::variant<Some, Err>;

bool isSuccess(const Result& result) {
    return std::visit([](auto&& r) -> bool {
        using T = std::decay_t<decltype(r)>;
        return std::is_same_v<T, Some>;
    }, result);
}

std::string err(const Result& result) {
    return std::visit([](auto&& r) -> std::string {
        using T = std::decay_t<decltype(r)>;
        if constexpr (std::is_same_v<T, Err>) {
            return r.err;
        } else return NULL;
    }, result);
}
