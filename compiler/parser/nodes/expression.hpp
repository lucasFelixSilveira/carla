#pragma once

#include "type.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <variant>

#include "../ctx.hpp"
#include "../../libs/morgana/types.hpp"

namespace carla {

    struct ExprContext;
    using InterpreterResult = std::variant<std::monostate, size_t, std::string>;
    using ExprBlock = std::vector<carla::ExprContext>;
    struct ExprContext {
        enum Kind { Value, Node, Block } kind;
        std::variant<pContext, void*, std::vector<ExprContext>> content;

        static ExprContext make_value(const pContext& ctx) {
            ExprContext e;
            e.kind = Value;
            e.content = ctx;
            return e;
        }

        static ExprContext make_node(void* node) {
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

        static ExprContext none() {
            return ExprContext();
        }
    };

    struct Expr {
        bool is_static;
        std::optional<carla::Type> result;
        ExprContext ast;
        InterpreterResult data;

        Expr(carla::ExprContext ast)
            : is_static(false),
              result(),
              ast(ast),
              data() {};

        Expr(carla::Type t, carla::ExprContext ast)
            : is_static(false),
              result(t),
              ast(ast),
              data() {};

        Expr()
            : is_static(false),
              ast(ExprContext::none()),
              result(),
              data() {}

        static Expr make_string(std::string str) {
            Expr e;
            e.is_static = true;
            e.result.emplace(carla::Type("int8*", morgana::ptr()));
            e.data = str;
            return e;
        }

        static Expr make_integer(size_t data) {
            Expr e;
            e.is_static = true;
            e.result.emplace(carla::Type("int", morgana::integer(0)));
            e.data = data;
            return e;
        }
    };
}
