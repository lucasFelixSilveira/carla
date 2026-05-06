#pragma once

#include "type.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <variant>

#include "../node.hpp"

namespace carla {
    struct ExprContext {
        enum Kind { Value, Node, Block } kind;
        std::variant<pContext, pNode, std::vector<ExprContext>> content;

        static ExprContext make_value(const pContext& ctx) {
            ExprContext e;
            e.kind = Value;
            e.content = ctx;
            return e;
        }

        static ExprContext make_node(const pNode& node) {
            ExprContext e;
            e.kind = Node;
            e.content = node;
            return e;
        }

        static ExprContext make_block(const std::vector<ExprContext>& block) {
            ExprContext e;
            e.kind = Block;
            e.content = block;
            return e;
        }
    };

    struct Expr {
        bool is_static;
        carla::Type result;
        ExprContext ast;
    };
}
