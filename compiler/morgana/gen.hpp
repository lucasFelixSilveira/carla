#pragma once

#include <cstdio>
#include <memory>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../parser/ast.hpp"
#include "../parser/parser.hpp"

#include "../libs/morgana/builder.hpp"
#include "../libs/morgana/context.hpp"

#define has(n) (i + n < nodes.size())

enum reason {
    var_declaration,
    return_statement,
    puts_statement
};

std::string generateMorganaCode(std::vector<pNode> nodes, Symt symbols, bool func) {
    Builder builder;
    Storage storage;
    std::unordered_map<std::string, long long> addr_record;

    std::stack<std::tuple<reason, std::variant<morgana::dynamic, std::shared_ptr<morgana::alloc>>>> expr_stack;

    long i = 0;
    for(; i < nodes.size(); i++ ) {
        pNode node = nodes[i];

        switch(node.kind) {
            case NodeKind::NODE_EXPRESSION: {
                auto [keyword, expr, block] = std::get<pExpression>(node.values);

                if( std::holds_alternative<morgana::expr::call_expr>(expr) ) {
                    auto call = std::get<morgana::expr::call_expr>(expr);
                    bool comma = false;
                    std::vector<std::string> args;
                    for( const auto& arg : call.args ) {
                        if( addr_record.find(arg) != addr_record.end() ) {
                            morgana::load l(storage, addr_record.at(arg));
                            builder << l.string();
                            args.push_back("_" + std::to_string(l.addr));
                        } else { args.push_back(arg); }
                    }
                    morgana::call c(call.func, args);
                    builder << c.string();
                }

                std::string comptime_string;
                if( std::holds_alternative<morgana::expr::single_expr>(expr) ) {
                    auto single = std::get<morgana::expr::single_expr>(expr);
                    if( single.constant ) { comptime_string = single.value; }
                }

                if( expr_stack.size() == 0 ) {};

                auto [ why, dest ] = expr_stack.top();
                if( why == puts_statement ) {
                    morgana::puts p(storage, comptime_string);
                    builder << p.string();
                }

                if( why == var_declaration ) {
                    auto val = std::get<std::shared_ptr<morgana::alloc>>(dest);
                    if(! comptime_string.empty() ) {
                        morgana::constant c(storage, comptime_string);
                        morgana::store s(val->addr, morgana::identifier::from(c.addr));

                        builder << c.string();
                        builder << s.string();
                    }
                }
            } break;

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
                        // morgana::desconstruct d(morgana::mics::that, lambda.argsn);
                        // ctx << d.string();

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

                // TODO! Comptime variables
                if( decl.complement == dUknownType ) {
                    continue;
                }

                /* Just build the type with the templates and somestuff like that */
                std::shared_ptr<morgana::type> type = builtin(&decl.type, assemble_special_symbol(&symbols, decl.ctx));
                morgana::alloc ptr(storage, type);
                addr_record.insert({ decl.name, ptr.addr });
                builder << ptr.string();

                /* Hopelesss declarations are not pushed onto the
                 * stack and also don't need to be instantiated */
                if( decl.complement == dHopeless ) continue;

                /* Non-hopeless declarations need to be instantiated */
                if( (!has(1)) || nodes[i + 1].kind != NodeKind::NODE_EXPRESSION )
                    CompilerOutputs::Fatal("A `hopefull` declaration needs a Expression before it!");

                /* Push the declaration and its allocated memory onto the stack */
                expr_stack.push({ var_declaration, ptr.shared() });
            } break;

            case NodeKind::NODE_MACRO: {
                Macro macro = std::get<Macro>(node.values);
                switch(macro.kind) {
                    case Macro::options::start: { builder << "comptime _start\n"; } break;
                    default: break;
                }
            } break;

            case NodeKind::NODE_RETURN: {
                std::cout << "return statement\n";
                SimpleStatement ret = std::get<SimpleStatement>(node.values);

                if( ret.hopeless ) {
                    morgana::ret r;
                    builder << r.string();
                    continue;
                }

                /* Non-hopeless declarations need to be instantiated */
                if( (!has(1)) || nodes[i + 1].kind != NodeKind::NODE_EXPRESSION )
                    CompilerOutputs::Fatal("A `hopefull` declaration needs a Expression before it!");

                /* Push the declaration and its allocated memory onto the stack */
                expr_stack.push({ return_statement, morgana::dynamic() });

            } break;

            case NodeKind::NODE_PUTS: {
                /* Non-hopeless declarations need to be instantiated */
                if( (!has(1)) || nodes[i + 1].kind != NodeKind::NODE_EXPRESSION )
                    CompilerOutputs::Fatal("A `hopefull` declaration needs a Expression before it!");

                /* Push the declaration and its allocated memory onto the stack */
                expr_stack.push({ puts_statement, morgana::dynamic() });

            } break;

            default: break;
        }
    }

    return builder.string();
};
