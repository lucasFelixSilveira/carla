#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <vector>
#include <variant>
#include "../parser/symbols.hpp"
#include "../parser/ast.hpp"
#include "types.hpp"
#include "../globals.hpp"
#include "../params.hpp"
#include "expressions.hpp"

std::string generateLLVMIR(const std::vector<pNode>& nodes, CompilerParams params ) {
    llvm::LLVMContext context;
    llvm::Module module(params.main, context);
    llvm::IRBuilder<> builder(context);
    Symbols symbolTable;

    std::map<std::string, llvm::AllocaInst*> namedValues; // Para armazenar variáveis locais
    llvm::Function* currentFunction = nullptr;
    llvm::Type* type = llvm::Type::getVoidTy(context);
    std::string funcName = "unnamed";

    int i = 0;
    while(i < nodes.size()) {
        auto first = nodes[i];
        pNode second;
        if( i + 1 < nodes.size() ) {
            second = nodes[i + 1];
        }

        // Function declaration
        if( first.kind == NODE_DECLARATION && second.kind == NODE_LAMBDA ) {
            pDeclaration decl = std::get<pDeclaration>(first.values);
            pLambda lamb = std::get<pLambda>(second.values);

            type = getLLVMType(decl.type.radical, context);
            funcName = decl.name;

            std::vector<llvm::Type*> paramTypes;
            std::vector<std::string> paramNames;
            for( const auto& argNode : lamb.args ) {
                if( argNode.kind == NODE_DECLARATION ) {
                    if( auto* argDecl = std::get_if<pDeclaration>(&argNode.values) ) {
                        paramTypes.push_back(getLLVMType(argDecl->type.radical, context));
                        paramNames.push_back(argDecl->name);
                    } else {
                        paramTypes.push_back(llvm::Type::getInt32Ty(context));
                        paramNames.push_back("arg_" + std::to_string(paramNames.size()));
                    }
                } else {
                    paramTypes.push_back(llvm::Type::getInt32Ty(context));
                    paramNames.push_back("arg_" + std::to_string(paramNames.size()));
                }
            }

            auto func = llvm::Function::Create(
                llvm::FunctionType::get(type, paramTypes, false),
                lamb.pub ? llvm::Function::ExternalLinkage : llvm::Function::PrivateLinkage,
                funcName,
                &module
            );

            currentFunction = func;
            symbolTable.add(funcName, Symbol(FUNCTION, decl.type));
            builder.SetInsertPoint(llvm::BasicBlock::Create(context, "entry", func));

            unsigned idx = 0;
            for( auto& arg : func->args() ) {
                if( idx < paramNames.size() ) {
                    arg.setName(paramNames[idx]);
                    llvm::AllocaInst* alloca = builder.CreateAlloca(paramTypes[idx], nullptr, paramNames[idx] + ".addr");
                    builder.CreateStore(&arg, alloca);
                    namedValues[paramNames[idx]] = alloca;

                    if( idx < lamb.args.size() ) {
                        if( auto* argDecl = std::get_if<pDeclaration>(&lamb.args[idx].values) ) {
                            symbolTable.add(paramNames[idx], Symbol(VARIABLE, VAR_PARAM, argDecl->type.radical, argDecl->type.bytes));
                        }
                    }
                }
                ++idx;
            }
        }
        // Processar declaração de variável
        else if (first.kind == NODE_DECLARATION) {
            pDeclaration decl = std::get<pDeclaration>(first.values);

            // Adicionar à tabela de símbolos
            symbolTable.add(decl.name, Symbol(decl.name, VARIABLE, VAR_LOCAL, decl.type.radical, decl.type.bytes));

            // Se estamos dentro de uma função, criar alocação na pilha
            if (currentFunction) {
                llvm::Type* varType = getLLVMType(decl.type.radical, context);
                llvm::AllocaInst* alloca = createEntryBlockAlloca(currentFunction, builder, decl.name, varType);
                namedValues[decl.name] = alloca;

                // Se temos uma inicialização (expressão)
                if (i + 1 < nodes.size() && nodes[i + 1].kind == NODE_EXPRESSION) {
                    pExpression expr = std::get<pExpression>(nodes[i + 1].values);
                    llvm::Value* initValue = generateExpression(expr, builder, context, module, symbolTable, namedValues);
                    builder.CreateStore(initValue, alloca);
                    i++; // Consumir o próximo nó (a expressão)
                }
            }
        }
        // Processar expressões soltas
        else if (first.kind == NODE_EXPRESSION) {
            pExpression expr = std::get<pExpression>(first.values);
            generateExpression(expr, builder, context, module, symbolTable, namedValues);
        }

        i++;
    }

    llvm::verifyModule(module, &llvm::errs());

    std::string ir;
    llvm::raw_string_ostream os(ir);
    module.print(os, nullptr);
    return ir;
}
