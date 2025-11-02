#pragma once

#include <memory>
#include <vector>

#include "../parser/ast.hpp"
#include "../parser/parser.hpp"

#include "../libs/morgana/builder.hpp"
#include "../libs/morgana/context.hpp"

#define has(n) (i + n < nodes.size())

std::string generateMorganaCode(std::vector<pNode> nodes, Symt symbols, bool func) {
    Builder builder;

    long i = 0;
    for(; i < nodes.size(); i++ ) {
        pNode node = nodes[i];

        switch(node.kind) {
            case NodeKind::NODE_DECLARATION: {

                if( (!func) && has(1) ) {
                    pNode next = nodes[i + 1];

                    if( next.kind == NodeKind::NODE_LAMBDA ) {
                        i++;

                        pDeclaration decl = std::get<pDeclaration>(node.values);
                        auto ret = std::get<std::shared_ptr<morgana::type>>(decl.type);

                        pLambda lambda = std::get<pLambda>(next.values);

                        symbols.entry();

                        std::vector<pNode> stmt = {};
                        auto body = std::get<std::vector<pContext>>(lambda.body.content);
                        Parser::checkSyntax(symbols, &stmt, body, false);

                        Context ctx;
                        morgana::desconstruct d(morgana::mics::that, lambda.argsn);
                        ctx << d.string();
                        ctx << generateMorganaCode(stmt, symbols, true);

                        morgana::function f(decl.name, ret, lambda.argst, ctx.string());
                        builder << f.string();

                        continue;
                    }
                }

                // pDeclaration decl = std::get<pDeclaration>(node.values);
                // std::tuple<std::string, int> result = LLVM::alloca(decl.type);
                // ctx << std::get<0>(result);
                // int dst = std::get<1>(result);

                // if( decl.complement == dHopeless ) continue;

                // if( (!has(1)) || nodes[i + 1].kind != NodeKind::NODE_EXPRESSION )
                //     CompilerOutputs::Fatal("A `hopefull` declaration needs a Expression before it!");

                // pNode expr = nodes[i + 1];
                // auto ptr = std::make_shared<pNode>(expr);
                // result = morg::expr(LLVM::toStr(decl.type), symbols, ptr);
                // ctx << std::get<0>(result) << morg::store(decl, symbols, std::get<1>(result), dst);
            } break;
            default: break;
        }
    }

    return builder.string();
};
