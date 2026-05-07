#pragma once

#include <variant>
#include "nodes/declaration.hpp"
#include "nodes/expression.hpp"
#include "nodes/statement.hpp"
#include "nodes/start.hpp"
#include "nodes/lambda.hpp"

#define CARLA_PNODE_CASE \
    X(carla::Decl, DECLARATION) \
    X(carla::Lambda, LAMBDA) \
    X(carla::Start, COMPTIME_START) \
    X(carla::Expr, EXPRESSION) \
    X(carla::Stmt, STATEMENT)

#define X(type, ...) , type
using pNode = std::variant<std::monostate CARLA_PNODE_CASE>;
#undef X

#define X(_, index) index,
enum pKind : int { _I = 0, CARLA_PNODE_CASE };
#undef X
