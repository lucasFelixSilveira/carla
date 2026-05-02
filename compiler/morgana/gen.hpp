#pragma once

#include <variant>
#include <vector>

#include "../parser/node.hpp"
#include "../parser/symbols.hpp"
#include "../parser/parser.hpp"

#include "../libs/morgana/builder.hpp"
#include "../libs/morgana.hpp"

std::string generateMorganaCode(std::vector<pNode> nodes, Symt& symbols, bool internal) {
    Builder builder;
    Storage storage;
    storage.variable.push(0);

    for( int index = 0; index < nodes.size(); index++ ) {
        pNode node = nodes[index];

        switch(node.index()) {
            case DECLARATION: {
                auto decl = std::get<carla::Decl>(node);

                if( decl.k == carla::Decl::Hopeless ) {
                    builder << morgana::alloc(&storage, decl.type.morgana);
                    continue;
                }

                if( (index + 1) < nodes.size() && nodes[index + 1].index() == LAMBDA ) {
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
                    ctx.~Context();
                    index++;
                    continue;
                }

                break;
            } break;
        }
    }

    return builder.string();
};
