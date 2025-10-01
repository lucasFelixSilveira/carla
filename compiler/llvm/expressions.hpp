#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Value.h>
#include <string>
#include <variant>
#include <memory>
#include "../parser/ast.hpp"
#include "../parser/symbols.hpp"
#include "types.hpp"
#include "../compiler_outputs.hpp"

// Função auxiliar para converter valores literais para o tipo LLVM correto
inline llvm::Value* convertLiteral(const std::variant<std::string, long long int, double, bool>& literal,
                                  llvm::IRBuilder<>& builder,
                                  llvm::LLVMContext& context) {
    if (std::holds_alternative<long long int>(literal)) {
        // Número inteiro
        long long int value = std::get<long long int>(literal);
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), value);
    } else if (std::holds_alternative<double>(literal)) {
        // Número de ponto flutuante
        double value = std::get<double>(literal);
        return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), value);
    } else if (std::holds_alternative<bool>(literal)) {
        // Booleano
        bool value = std::get<bool>(literal);
        return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), value ? 1 : 0);
    } else {
        // String
        std::string str = std::get<std::string>(literal);
        return builder.CreateGlobalStringPtr(str);
    }
}

// Função principal para gerar código LLVM para expressões
inline llvm::Value* generateExpression(
    const pExpression& expr,
    llvm::IRBuilder<>& builder,
    llvm::LLVMContext& context,
    llvm::Module& module,
    Symbols& symbolTable,
    std::map<std::string, llvm::AllocaInst*>& namedValues
) {
    switch (expr.kind) {
        case EXPR_LITERAL: {
            return convertLiteral(std::get<std::variant<std::string, long long int, double, bool>>(expr.content), builder, context);
        }

        case EXPR_VARIABLE: {
            Symbol sym = std::get<Symbol>(expr.content);

            // Verificar se a variável existe no escopo atual
            if (namedValues.find(sym.name) != namedValues.end()) {
                return builder.CreateLoad(getLLVMType(sym.radical, context), namedValues[sym.name], sym.name);
            } else {
                CompilerOutputs::Fatal("Variável não encontrada: " + sym.name);
                // Retornar um valor padrão para evitar erros de compilação
                return llvm::ConstantInt::get(getLLVMType(i32, context), 0);
            }
        }

        case EXPR_BINARY: {
            auto [opKind, lhsNode, rhsNode] = std::get<std::tuple<TokenKind, std::shared_ptr<pNode>, std::shared_ptr<pNode>>>(expr.content);

            // Verificar se os nós são expressões
            if (lhsNode->kind != NODE_EXPRESSION || rhsNode->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Expressão binária inválida - operandos devem ser expressões");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& lhsExpr = std::get<pExpression>(lhsNode->values);
            pExpression& rhsExpr = std::get<pExpression>(rhsNode->values);

            // Gerar código para cada lado da expressão
            llvm::Value* lhs = generateExpression(lhsExpr, builder, context, module, symbolTable, namedValues);
            llvm::Value* rhs = generateExpression(rhsExpr, builder, context, module, symbolTable, namedValues);

            // Verificar se ambos os valores são válidos
            if (!lhs || !rhs) {
                CompilerOutputs::Fatal("Falha ao gerar código para expressão binária");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            // Realizar a operação binária apropriada
            switch (opKind) {
                case PLUS:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        // Converter operandos para ponto flutuante se necessário
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFAdd(lhs, rhs, "addtmp");
                    }
                    return builder.CreateAdd(lhs, rhs, "addtmp");
                case MINUS:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        // Converter operandos para ponto flutuante se necessário
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFSub(lhs, rhs, "subtmp");
                    }
                    return builder.CreateSub(lhs, rhs, "subtmp");
                case STAR:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFMul(lhs, rhs, "multmp");
                    }
                    return builder.CreateMul(lhs, rhs, "multmp");
                case SLASH:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFDiv(lhs, rhs, "divtmp");
                    }
                    return builder.CreateSDiv(lhs, rhs, "divtmp");
                // case MOD:
                //     if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                //         if (!lhs->getType()->isFloatingPointTy()) {
                //             lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         if (!rhs->getType()->isFloatingPointTy()) {
                //             rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         return builder.CreateFRem(lhs, rhs, "modtmp");
                //     }
                //     return builder.CreateSRem(lhs, rhs, "modtmp");
                case EQUAL:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFCmpOEQ(lhs, rhs, "eqtmp");
                    }
                    return builder.CreateICmpEQ(lhs, rhs, "eqtmp");
                // case NOT_EQUAL:
                //     if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                //         if (!lhs->getType()->isFloatingPointTy()) {
                //             lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         if (!rhs->getType()->isFloatingPointTy()) {
                //             rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         return builder.CreateFCmpONE(lhs, rhs, "neqtmp");
                //     }
                //     return builder.CreateICmpNE(lhs, rhs, "neqtmp");
                // case LESS_THAN:
                //     if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                //         if (!lhs->getType()->isFloatingPointTy()) {
                //             lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         if (!rhs->getType()->isFloatingPointTy()) {
                //             rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         return builder.CreateFCmpOLT(lhs, rhs, "lttmp");
                //     }
                //     return builder.CreateICmpSLT(lhs, rhs, "lttmp");
                // case GREATER_THAN:
                //     if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                //         if (!lhs->getType()->isFloatingPointTy()) {
                //             lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         if (!rhs->getType()->isFloatingPointTy()) {
                //             rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         return builder.CreateFCmpOGT(lhs, rhs, "gttmp");
                //     }
                //     return builder.CreateICmpSGT(lhs, rhs, "gttmp");
                case LESS_EQUAL:
                    if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                        if (!lhs->getType()->isFloatingPointTy()) {
                            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        if (!rhs->getType()->isFloatingPointTy()) {
                            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                        }
                        return builder.CreateFCmpOLE(lhs, rhs, "letmp");
                    }
                    return builder.CreateICmpSLE(lhs, rhs, "letmp");
                // case GREATER_EQUAL:
                //     if (lhs->getType()->isFloatingPointTy() || rhs->getType()->isFloatingPointTy()) {
                //         if (!lhs->getType()->isFloatingPointTy()) {
                //             lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         if (!rhs->getType()->isFloatingPointTy()) {
                //             rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context), "casttmp");
                //         }
                //         return builder.CreateFCmpOGE(lhs, rhs, "getmp");
                //     }
                //     return builder.CreateICmpSGE(lhs, rhs, "getmp");
                case AND:
                    return builder.CreateAnd(lhs, rhs, "andtmp");
                case OR:
                    return builder.CreateOr(lhs, rhs, "ortmp");
                // case BIT_AND:
                //     return builder.CreateAnd(lhs, rhs, "bitandtmp");
                // case BIT_OR:
                //     return builder.CreateOr(lhs, rhs, "bitortmp");
                // case BIT_XOR:
                //     return builder.CreateXor(lhs, rhs, "bitxortmp");
                // case LEFT_SHIFT:
                //     return builder.CreateShl(lhs, rhs, "shltmp");
                // case RIGHT_SHIFT:
                //     return builder.CreateLShr(lhs, rhs, "shrtmp");
                // case ASSIGN:
                //     // Caso especial para atribuição
                //     if (lhsExpr.kind == EXPR_VARIABLE) {
                //         Symbol sym = std::get<Symbol>(lhsExpr.content);
                //         if (namedValues.find(sym.name) != namedValues.end()) {
                //             builder.CreateStore(rhs, namedValues[sym.name]);
                //             symbolTable.setInitialized(sym.name);
                //             return rhs; // Retorna o valor atribuído
                //         } else {
                //             CompilerOutputs::Fatal("Variável não encontrada para atribuição: " + sym.name);
                //         }
                //     } else {
                //         CompilerOutputs::Fatal("Lado esquerdo de atribuição deve ser uma variável");
                //     }
                //     return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
                default:
                    CompilerOutputs::Fatal("Operador binário não suportado");
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }
        }

        case EXPR_UNARY: {
            auto [opKind, operandNode] = std::get<std::tuple<TokenKind, std::shared_ptr<pNode>>>(expr.content);

            // Verificar se o nó é uma expressão
            if (operandNode->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Expressão unária inválida - operando deve ser uma expressão");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& operandExpr = std::get<pExpression>(operandNode->values);

            // Gerar código para o operando
            llvm::Value* operand = generateExpression(operandExpr, builder, context, module, symbolTable, namedValues);

            // Verificar se o valor é válido
            if (!operand) {
                CompilerOutputs::Fatal("Falha ao gerar código para expressão unária");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            // Realizar a operação unária apropriada
            switch (opKind) {
                case MINUS:
                    if (operand->getType()->isFloatingPointTy()) {
                        return builder.CreateFNeg(operand, "negtmp");
                    }
                    return builder.CreateNeg(operand, "negtmp");
                // case BIT_NOT:
                //     return builder.CreateNot(operand, "nottmp");
                case PLUS: // Unary plus é uma no-op
                    return operand;
                default:
                    CompilerOutputs::Fatal("Operador unário não suportado");
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }
        }

        case EXPR_CALL: {
            auto [callee, args] = std::get<std::tuple<std::shared_ptr<pNode>, std::vector<std::shared_ptr<pNode>>>>(expr.content);

            // Verificar se o nó é uma expressão
            if (callee->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Chamada de função inválida - callee deve ser uma expressão");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& calleeExpr = std::get<pExpression>(callee->values);

            // Verificar se o callee é uma variável (nome de função)
            if (calleeExpr.kind != EXPR_VARIABLE) {
                CompilerOutputs::Fatal("Chamada de função inválida - callee deve ser uma variável");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            Symbol calleeSymbol = std::get<Symbol>(calleeExpr.content);

            // Buscar a função no módulo
            llvm::Function* function = module.getFunction(calleeSymbol.name);
            if (!function) {
                CompilerOutputs::Fatal("Função não encontrada: " + calleeSymbol.name);
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            // Verificar número de argumentos
            if (function->arg_size() != args.size()) {
                CompilerOutputs::Fatal("Número incorreto de argumentos para função: " + calleeSymbol.name);
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            // Gerar código para argumentos
            std::vector<llvm::Value*> argValues;
            for (size_t i = 0; i < args.size(); i++) {
                if (args[i]->kind != NODE_EXPRESSION) {
                    CompilerOutputs::Fatal("Argumento inválido para chamada de função");
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
                }

                pExpression& argExpr = std::get<pExpression>(args[i]->values);
                llvm::Value* argValue = generateExpression(argExpr, builder, context, module, symbolTable, namedValues);

                if (!argValue) {
                    CompilerOutputs::Fatal("Falha ao gerar código para argumento de função");
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
                }

                argValues.push_back(argValue);
            }

            return builder.CreateCall(function, argValues, "calltmp");
        }

        case EXPR_MEMBER: {
            auto [object, member] = std::get<std::tuple<std::shared_ptr<pNode>, std::string>>(expr.content);

            // Verificar se o nó é uma expressão
            if (object->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Acesso a membro inválido - objeto deve ser uma expressão");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& objectExpr = std::get<pExpression>(object->values);

            // Gerar código para o objeto
            llvm::Value* objectValue = generateExpression(objectExpr, builder, context, module, symbolTable, namedValues);

            // TODO: Implementar acesso a membros de estruturas
            CompilerOutputs::Fatal("Acesso a membro ainda não implementado");
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        }

        case EXPR_INDEX: {
            auto [array, index] = std::get<std::tuple<std::shared_ptr<pNode>, std::shared_ptr<pNode>>>(expr.content);

            // Verificar se os nós são expressões
            if (array->kind != NODE_EXPRESSION || index->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Acesso a índice inválido - array e índice devem ser expressões");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& arrayExpr = std::get<pExpression>(array->values);
            pExpression& indexExpr = std::get<pExpression>(index->values);

            // Gerar código para array e índice
            llvm::Value* arrayValue = generateExpression(arrayExpr, builder, context, module, symbolTable, namedValues);
            llvm::Value* indexValue = generateExpression(indexExpr, builder, context, module, symbolTable, namedValues);

            // Verificar se ambos os valores são válidos
            if (!arrayValue || !indexValue) {
                CompilerOutputs::Fatal("Falha ao gerar código para acesso a índice");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            // TODO: Implementar acesso a arrays
            CompilerOutputs::Fatal("Acesso a índice ainda não implementado");
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        }

        // case EXPR_TERNARY: {
        //     auto [condition, thenExpr, elseExpr] = std::get<std::tuple<std::shared_ptr<pNode>, std::shared_ptr<pNode>, std::shared_ptr<pNode>>>(expr.content);

        //     // Verificar se os nós são expressões
        //     if (condition->kind != NODE_EXPRESSION || thenExpr->kind != NODE_EXPRESSION || elseExpr->kind != NODE_EXPRESSION) {
        //         CompilerOutputs::Fatal("Operador ternário inválido - condição, then e else devem ser expressões");
        //         return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        //     }

        //     pExpression& condExpr = std::get<pExpression>(condition->values);
        //     pExpression& thenExpression = std::get<pExpression>(thenExpr->values);
        //     pExpression& elseExpression = std::get<pExpression>(elseExpr->values);

        //     // Gerar código para condição
        //     llvm::Value* condValue = generateExpression(condExpr, builder, context, module, symbolTable, namedValues);

        //     // Comparar a condição com zero para obter valor booleano
        //     llvm::Value* condBool = builder.CreateICmpNE(
        //         condValue,
        //         llvm::ConstantInt::get(condValue->getType(), 0),
        //         "condtmp"
        //     );

        //     // Criar blocos para then, else e merge
        //     llvm::Function* func = builder.GetInsertBlock()->getParent();
        //     llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", func);
        //     llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
        //     llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "merge");

        //     // Criar branch condicional
        //     builder.CreateCondBr(condBool, thenBB, elseBB);

        //     // Gerar código para then
        //     builder.SetInsertPoint(thenBB);
        //     llvm::Value* thenValue = generateExpression(thenExpression, builder, context, module, symbolTable, namedValues);
        //     builder.CreateBr(mergeBB);
        //     thenBB = builder.GetInsertBlock(); // Pode ter mudado

        //     // Gerar código para else
        //     func->getBasicBlockList().push_back(elseBB);
        //     builder.SetInsertPoint(elseBB);
        //     llvm::Value* elseValue = generateExpression(elseExpression, builder, context, module, symbolTable, namedValues);
        //     builder.CreateBr(mergeBB);
        //     elseBB = builder.GetInsertBlock(); // Pode ter mudado

        //     // Gerar merge
        //     func->getBasicBlockList().push_back(mergeBB);
        //     builder.SetInsertPoint(mergeBB);
        //     llvm::PHINode* phi = builder.CreatePHI(thenValue->getType(), 2, "ternary");
        //     phi->addIncoming(thenValue, thenBB);
        //     phi->addIncoming(elseValue, elseBB);

        //     return phi;
        // }

        case EXPR_GROUP: {
            std::shared_ptr<pNode> innerExpr = std::get<std::shared_ptr<pNode>>(expr.content);

            // Verificar se o nó é uma expressão
            if (innerExpr->kind != NODE_EXPRESSION) {
                CompilerOutputs::Fatal("Expressão agrupada inválida - deve ser uma expressão");
                return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            }

            pExpression& groupExpr = std::get<pExpression>(innerExpr->values);

            // Simplesmente gerar código para a expressão interna
            return generateExpression(groupExpr, builder, context, module, symbolTable, namedValues);
        }

        default:
            CompilerOutputs::Fatal("Tipo de expressão não suportado");
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    }
}

// Função para criar alocação de variável local na pilha
inline llvm::AllocaInst* createEntryBlockAlloca(
    llvm::Function* function,
    llvm::IRBuilder<>& builder,
    const std::string& varName,
    llvm::Type* type
) {
    // Cria um builder para inserir instruções no início do bloco de entrada
    llvm::IRBuilder<> TmpBuilder(&function->getEntryBlock(),
                                function->getEntryBlock().begin());
    return TmpBuilder.CreateAlloca(type, nullptr, varName);
}
