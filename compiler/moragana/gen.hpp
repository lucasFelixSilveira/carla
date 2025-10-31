#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "../parser/ast.hpp"
#include "../params.hpp"

#include "./morgana.hpp"

#define has(n) (i + n < nodes.size())

std::string generateMorganaCode(std::vector<pNode> nodes, Symbols symbols, bool func) {
    std::stringstream ctx;

    uint64_t i = 0;
    for(; i < nodes.size(); i++ ) {
        pNode node = nodes[i];

        switch(node.kind) {
            case NodeKind::NODE_DECLARATION: {

                if( (!func) && has(1) ) {
                    pNode next = nodes[i + 1];

                    if( next.kind == NodeKind::NODE_LAMBDA ) {
                        i++;
                        ctx << morg::lambda(symbols, node, next);
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

    return ctx.str();
};
