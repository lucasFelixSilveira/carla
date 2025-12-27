#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "../parser/ast.hpp"
#include "../parser/parser.hpp"

#include "../libs/morgana/builder.hpp"
#include "../libs/morgana/context.hpp"

#define has(n) (i + n < nodes.size())

std::string generateMorganaCode(std::vector<pNode> nodes, Symt symbols, bool func) {
    Builder builder;
    Storage storage;

    long i = 0;
    for(; i < nodes.size(); i++ ) {
        pNode node = nodes[i];

        switch(node.kind) {
            case NodeKind::NODE_DECLARATION: {

                if( (!func) && has(1) ) {
                    pNode next = nodes[i + 1];

                    if( next.kind == NodeKind::NODE_LAMBDA ) {
                        i++;

                        /*
                         * Uses the declaration node to collect the return type
                         * of the lambda whose it was declared
                         */
                        pDeclaration decl = std::get<pDeclaration>(node.values);
                        auto ret = std::get<std::shared_ptr<morgana::type>>(decl.type);

                        /*
                         * Make the same, but now for the lambda body and
                         * the arguments stuff
                         */
                        pLambda lambda = std::get<pLambda>(next.values);

                        /* Enter one more scope */
                        symbols.entry();

                        /*
                         * Generate the statement vector to the lambda body
                         * and then check the syntax to validate it
                         */
                        std::vector<pNode> stmt = {};
                        auto body = std::get<std::vector<pContext>>(lambda.body.content);
                        Parser::checkSyntax(symbols, &stmt, body, false);

                        /*
                         * Create the lambda body context and generates
                         * the IR into the lambda;
                         *
                         * Also do the arguments destruction, for the
                         * renaming of the arguments
                         */
                        Context ctx;
                        morgana::desconstruct d(morgana::mics::that, lambda.argsn);
                        ctx << d.string();
                        ctx << generateMorganaCode(stmt, symbols, true);

                        /*
                         * Finally, create the lambda, define all the required
                         * info and put it into the builder
                         */
                        morgana::function f(decl.name, ret, lambda.argst, ctx.string());
                        builder << f.string();

                        symbols.exit();

                        /* Jumps to the next iteration */
                        continue;
                    }
                }

                pDeclaration decl = std::get<pDeclaration>(node.values);
                // comtime
                if( decl.complement == dUknownType ) {
                    continue;
                }

                std::shared_ptr<morgana::type> type = builtin(&decl.type, assemble_special_symbol(&symbols, decl.ctx));

                morgana::alloc ptr(storage, type);

                builder << ptr.string();
                if( decl.complement == dHopeless ) continue;

                if( (!has(1)) || nodes[i + 1].kind != NodeKind::NODE_EXPRESSION )
                    CompilerOutputs::Fatal("A `hopefull` declaration needs a Expression before it!");
            } break;
            default: break;
        }
    }

    return builder.string();
};
