#pragma once

#include <iostream>
#include <stack>
#include <variant>
#include <vector>

#include "../parser/node.hpp"
#include "../parser/symbols.hpp"
#include "../parser/parser.hpp"

#include "../libs/morgana/builder.hpp"
#include "../libs/morgana.hpp"

enum reason_t { VAR_DECLARATION };
using variable_id = size_t;
using declaration_t = std::tuple<variable_id, morgana::type>;
std::stack<std::tuple<reason_t, std::variant<
    std::monostate,
    declaration_t
>>> stack_reason;

std::string generateMorganaCode(std::vector<pNode> nodes, Symt& symbols, bool internal) {
    Builder builder;
    Storage storage;
    storage.variable.push(0);

    for( int index = 0; index < nodes.size(); index++ ) {
        pNode node = nodes[index];

        switch(node.index()) {
            case COMPTIME_START: builder << morgana::comptime("_start"); break;
            case EXPRESSION: {
                auto expr = std::get<carla::Expr>(node);

                bool already_d_make = false;
                if( expr.is_static && std::holds_alternative<std::string>(expr.data) ) {
                    builder << morgana::static_declaration(&storage, std::get<std::string>(expr.data));
                    already_d_make = true;
                }

                auto [ reason, data ] = stack_reason.top();
                stack_reason.pop();
                switch(reason) {
                    case VAR_DECLARATION: {
                        if( already_d_make ) {
                            builder << morgana::store(morgana::last(&storage, "alloc"), morgana::last(&storage, "expr"));
                            continue;
                        }

                        if( expr.is_static && std::holds_alternative<size_t>(expr.data) )
                        /* -> */ builder << morgana::store_literal(morgana::last(&storage, "alloc"), std::get<size_t>(expr.data));

                        continue;
                    } break;
                }
            } break;
            case DECLARATION: {
                auto decl = std::get<carla::Decl>(node);

                if( decl.k == carla::Decl::Hopefull && (index + 1) < nodes.size() && nodes[index + 1].index() == LAMBDA ) {
                    auto lambda = std::get<carla::Lambda>(nodes[index + 1]);
                    std::vector<pNode> statement;
                    Parser::checkSyntax(symbols, &statement, lambda.body, false);

                    std::vector<morgana::type> types;
                    std::vector<std::string> identifiers;
                    for( auto& [type, identifier] : lambda.args ) {
                        types.push_back(type.morgana);
                        identifiers.push_back(identifier);
                    }

                    Context ctx;
                    ctx << generateMorganaCode(statement, symbols, true);
                    builder << morgana::function(&storage, decl.identiifer, decl.type.morgana, types, ctx);
                    index++;
                    continue;
                }

                builder << morgana::alloc(&storage, decl.type.morgana);
                if( decl.k == carla::Decl::Hopefull ) stack_reason.push({
                    VAR_DECLARATION,
                    declaration_t { morgana::last(&storage, "alloc"), decl.type.morgana }
                });

                break;
            } break;
            case STATEMENT: {
                auto stmt = std::get<carla::Stmt>(node);
                switch(stmt.data) {
                    case carla::STMT_PUTS: {
                        auto err = [](){ CompilerOutputs::Fatal("Expected a string static expression after puts statement"); };
                        if( (index + 1) >= nodes.size() ) err();

                        auto expr = std::get<carla::Expr>(nodes[index + 1]);
                        if(! expr.is_static ) err();

                        if(! std::holds_alternative<std::string>(expr.data) ) err();
                        builder << morgana::puts(&storage, std::get<std::string>(expr.data));
                        index++;
                    } break;
                }
            } break;
        }
    }

    return builder.string();
};
