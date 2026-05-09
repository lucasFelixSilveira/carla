#pragma once

#include "../pattern.hpp"
#include "../nodes/expression.hpp"
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

size_t precedence(TokenKind kind) {
    switch(kind) {
        case TokenKind::PLUS:
        case TokenKind::MINUS: return 1;
        case TokenKind::STAR:
        case TokenKind::SLASH: return 2;
        case TokenKind::SHIFT_LEFT:
        case TokenKind::SHIFT_RIGHT:
        case TokenKind::LOR:
        case TokenKind::LAND:
        case TokenKind::XOR: return 3;
        default: return 0;
    }
}

bool is_operator(TokenKind kind) {
    return kind == PLUS || kind == MINUS || kind == STAR || kind == SLASH || kind == SLASH_SLASH
        || kind == SHIFT_LEFT || kind == SHIFT_RIGHT
        || kind == LOR || kind == LAND || kind == XOR;
}

bool is_expr_end_keyword(TokenKind kind)
{ return kind == TokenKind::ELSE; }

std::tuple<bool, pNode> parse_node(CARLA_PATTERN_ARGUMENTS) {
    Result d = pattern(CARLA_PATTERN_EXPORT, true);
    if( isSuccess(d) ) {
        pNode cp = *result;
        result->~pNode();
        return std::make_tuple(true, cp);
    }
    return std::make_tuple(false, std::monostate());
}

std::tuple<bool, carla::ExprContext> make_ast(CARLA_PATTERN_ARGUMENTS);
std::tuple<bool, carla::InterpreterResult> interpreter(CARLA_PATTERN_ARGUMENTS, carla::ExprContext& ast);

bool expression(CARLA_PATTERN_ARGUMENTS) {
    CARLA_PATTERN_STARTS(bool, false);
    CARLA_PEEK_NEXT(first, _default);
    auto [success_ast, ast] = make_ast(CARLA_PATTERN_EXPORT);
    if(! success_ast ) CARLA_RETURN_DEFAULT;

    auto [success_interpreter, val] = interpreter(CARLA_PATTERN_EXPORT, ast);
    if( success_interpreter ) {
        const int EXPR_INTEGER = 1, EXPR_STRING = 2;
        std::optional<carla::Expr> expr;
        switch(val.index()) {
            case EXPR_INTEGER: {
                int data = std::get<size_t>(val);
                std::cout << "Integer: " << data << "\n";
                expr.emplace(carla::Expr::make_integer(data));
            } break;
            case EXPR_STRING: {
                std::string data = std::get<std::string>(val);
                std::cout << "String: " << data << "\n";
                expr.emplace(carla::Expr::make_string(data));
            } break;
        }

        result->~pNode();
        new(result) pNode(expr.value());
    }
    return true;
}

std::tuple<bool, std::string> interpreter_string(CARLA_PATTERN_ARGUMENTS, carla::ExprContext& root) {
    std::function<std::string(carla::ExprContext&)> interpreter = [&](carla::ExprContext& ast) -> std::string {
        switch(ast.kind) {
            case carla::ExprContext::Node: throw std::runtime_error("_");
            case carla::ExprContext::Value: {
                auto ctx = std::get<pContext>(ast.content);
                auto val = std::get<Token>(ctx.content);
                if( val.kind != STRING ) throw std::runtime_error("_");
                return val.lexeme.substr(1, val.lexeme.size() - 2);
            } break;
            case carla::ExprContext::Block: {
                auto block = std::get<carla::ExprBlock>(ast.content);

                auto lhs = block[0],
                     rhs = block[2],
                     op  = block[1];

                auto lhs_val = interpreter(lhs),
                     rhs_val = interpreter(rhs);

                auto ctx = std::get<pContext>(op.content);
                auto op_tk = std::get<Token>(ctx.content);

                std::string result;
                switch(op_tk.kind) {
                    case PLUS: result = lhs_val + rhs_val; break;
                    default: throw std::runtime_error("_");
                }

                return result;
            };
        }
    };

    try { return std::make_tuple(true, interpreter(root)); }
    catch(const std::exception& _) { return std::make_tuple(false, std::string()); }
}

std::tuple<bool, size_t> interpreter_integer(CARLA_PATTERN_ARGUMENTS, carla::ExprContext& root) {
    std::function<size_t(carla::ExprContext&)> interpreter = [&](carla::ExprContext& ast) -> size_t {
        switch(ast.kind) {
            case carla::ExprContext::Node: throw std::runtime_error("_");
            case carla::ExprContext::Value: {
                auto ctx = std::get<pContext>(ast.content);
                auto val = std::get<Token>(ctx.content);
                if( val.kind != INTEGER ) throw std::runtime_error("_");
                return std::stoll(val.lexeme);
            } break;
            case carla::ExprContext::Block: {
                auto block = std::get<carla::ExprBlock>(ast.content);

                auto lhs = block[0],
                     rhs = block[2],
                     op  = block[1];

                auto lhs_val = interpreter(lhs),
                     rhs_val = interpreter(rhs);

                auto ctx = std::get<pContext>(op.content);
                auto op_tk = std::get<Token>(ctx.content);

                size_t result;
                switch(op_tk.kind) {
                    case PLUS: result = lhs_val + rhs_val; break;
                    case MINUS: result = lhs_val - rhs_val; break;
                    case STAR: result = lhs_val * rhs_val; break;
                    case SLASH_SLASH: result = (lhs_val - lhs_val % rhs_val) / rhs_val; break;
                    case SHIFT_LEFT: result = lhs_val << rhs_val; break;
                    case SHIFT_RIGHT: result = lhs_val >> rhs_val; break;
                    case XOR: result = lhs_val ^ rhs_val; break;
                    case LAND: result = lhs_val & rhs_val; break;
                    case LOR: result = lhs_val | rhs_val; break;
                    case LNOT: result = ~lhs_val; break;
                    default: throw std::runtime_error("_");
                }

                return result;
            };
        }
    };

    try { return std::make_tuple(true, interpreter(root)); }
    catch(const std::exception& _) { return std::make_tuple(false, 0); }
}

std::tuple<bool, carla::InterpreterResult> interpreter(CARLA_PATTERN_ARGUMENTS, carla::ExprContext& ast) {
    auto [str_success, str_result] = interpreter_string(CARLA_PATTERN_EXPORT, ast);
    if( str_success ) return std::make_tuple(true, str_result);

    auto [integer_success, integer_result] = interpreter_integer(CARLA_PATTERN_EXPORT, ast);
    if( integer_success ) return std::make_tuple(true, integer_result);

    return std::make_tuple(false, std::monostate());
}

void reduce_once(std::vector<carla::ExprContext>& values, std::vector<Token>& ops) {
    Token op = ops.back();
    ops.pop_back();

    carla::ExprContext rhs = values.back();
    values.pop_back();

    carla::ExprContext lhs = values.back();
    values.pop_back();

    std::vector<carla::ExprContext> blockContent;
    blockContent.push_back(lhs);

    pContext opCtx;
    opCtx.kind = Common;
    opCtx.content = op;
    blockContent.push_back(carla::ExprContext::make_value(opCtx));

    blockContent.push_back(rhs);

    values.push_back(carla::ExprContext::make_block(blockContent));
}

void reorder(std::vector<carla::ExprContext>& sub) {
    std::vector<carla::ExprContext> values;
    std::vector<Token> ops;

    for( size_t i = 0; i < sub.size(); i++ ) {
        auto& node = sub[i];
        if( node.kind == carla::ExprContext::Block ) {
            values.push_back(node);
            continue;
        }

        if( node.kind == carla::ExprContext::Node ) {
            values.push_back(node);
            continue;
        }

        auto ctx = std::get<pContext>(node.content);
        if( ctx.kind == Block ) {
            values.push_back(node);
            continue;
        }

        auto tk = std::get<Token>(ctx.content);

        if( is_operator(tk.kind) ) {
            while((! ops.empty()) && precedence(ops.back().kind) >= precedence(tk.kind))
            /* -> */ reduce_once(values, ops);
            ops.push_back(tk);
            continue;
        }

        values.push_back(node);
    }

    while(!ops.empty()) reduce_once(values, ops);

    sub.clear();
    if(! values.empty() ) sub.push_back(values.back());
}

void print_expr_context(const carla::ExprContext& expr, int level = 0);

std::tuple<bool, carla::ExprContext> make_ast(CARLA_PATTERN_ARGUMENTS) {
    auto d = std::make_tuple(false, carla::ExprContext());
    CARLA_PATTERN_STARTS(auto, d);

    std::vector<carla::ExprContext> sub;
    bool endded = false;

    while((*index) < ctx->size()) {
        auto [success, node] = parse_node(CARLA_PATTERN_EXPORT);

        if( success ) {
            sub.push_back(carla::ExprContext::make_node((void*)(&node)));
            continue;
        }

        auto ctxNode = (*ctx)[(*index)++];

        if( ctxNode.kind == Common && std::get<Token>(ctxNode.content).kind == TokenKind::SEMICOLON ) {
            endded = true;
            break;
        }

        sub.push_back(carla::ExprContext::make_value(ctxNode));
    }

    if(! endded ) CARLA_RETURN_DEFAULT;
    if( sub.empty() ) CARLA_RETURN_DEFAULT;

    reorder(sub);

    if(! sub.empty() ) { print_expr_context(sub[0]); }
    return { true, sub[0] };
}

void print_expr_context(const carla::ExprContext& expr, int level) {
    std::string indent(level * 2, ' ');

    switch(expr.kind) {
        case carla::ExprContext::Value: {
            auto& ctx = std::get<pContext>(expr.content);
            if( ctx.kind == Common ) {
                auto tk = std::get<Token>(ctx.content);
                std::cout << indent << "Value (Token): " << tk.to_string() << "\n";
            }
            else if( ctx.kind == Block ) {
                auto& block = std::get<std::vector<pContext>>(ctx.content);
                std::cout << indent << "Value (pContext Block) with " << block.size() << " elements\n";
                for (auto item : block ) {
                    if( item.kind == Common ) {
                        auto tk = std::get<Token>(item.content);
                        std::cout << indent << "  - " << tk.to_string() << "\n";
                    }
                }
            }
        } break;

        case carla::ExprContext::Node: {
            auto& node = std::get<void*>(expr.content);
            std::cout << indent << "Node: " << (*((pNode*)node)).index() << "\n";
        } break;

        case carla::ExprContext::Block: {
            auto& block = std::get<std::vector<carla::ExprContext>>(expr.content);
            std::cout << indent << "Block with " << block.size() << " elements:\n";
            for(auto& item : block) {
                print_expr_context(item, level + 1);
            }
        } break;
    }
}
