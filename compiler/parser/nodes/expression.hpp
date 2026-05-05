#pragma once

#include "type.hpp"
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <variant>
#include <vector>
namespace carla {
    enum AstNodeKind {
        Literal,
        Binary,
        Mathematic,
        FunctionCall
    };

    struct AST;
    using ASTValue = std::variant<std::monostate, std::string, size_t, std::shared_ptr<AST>>;
    struct AST {
        AstNodeKind kind;
        size_t operation;
        ASTValue lhs;
        ASTValue rhs;
        AST() = default;
    };

    struct Expr {
        bool is_static;
        carla::Type result;
        AST ast;
    };
}
