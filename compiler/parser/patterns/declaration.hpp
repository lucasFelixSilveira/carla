#pragma once

#include "../ast.hpp"
#include "../symbols.hpp"
#include <memory>
#include <variant>

// bool constDeclaration(pNode *result, Symbols *sym, long unsigned int *index, const std::vector<pContext>* ctx);
// bool letDeclaration(pNode *result, Symbols *sym, long unsigned int *index, const std::vector<pContext>* ctx);

bool declaration(pNode *result, Symt *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
    Token tk = std::get<Token>((*ctx)[*index].content);
    Symbol* type;

    // if( tk.kind == CONST ) return constDeclaration(result, sym, index, ctx);
    // if( tk.kind == LET ) return letDeclaration(result, sym, index, ctx);

    // Temos certeza que o valor é um identificador por causa da checagem
    // anterior feita pelo 'pattern.hpp'
    if( (type = sym->findSymbol(tk.lexeme)) == nullptr ) return false;
    if(! std::holds_alternative<std::shared_ptr<morgana::type>>(*type) ) return false;

    /// TODO: checagem de ponteiros
    /// TODO: checagem de arrays

    pContext identifier = (*ctx)[*index + 1];
    if( identifier.kind != Common ) return false;
    Token identifierToken = std::get<Token>(identifier.content);
    if( identifierToken.kind != IDENTIFIER ) return false;
    if( identifierToken.lexeme.empty() ) return false;

    // Checagem do tipo da declaração
    pContext symbol = (*ctx)[*index + 2];
    if( symbol.kind != Common ) return false;
    Token symbolToken = std::get<Token>(symbol.content);

    switch(symbolToken.kind) {
    case COLON:
    case SEMICOLON:
    case EQUAL: {
        pDeclaration_e declarationType =
            (symbolToken.kind == COLON) ? dIterable :
            (symbolToken.kind == SEMICOLON) ? dHopeless :
            dHopefull;

        // Adicionar variável à tabela de símbolos durante o parsing
        std::shared_ptr<morgana::type> into = std::get<std::shared_ptr<morgana::type>>(*type);
        morgana::variable v(identifierToken.lexeme, into, true);
        sym->addSymbol(identifierToken.lexeme, v.shared());

        *result = pNode(declarationType, *type, identifierToken.lexeme);
        *index += 2;
        return true;
    };

    case COLON_EQUAL: {
        // := só pode ser usado com let/let mut - rejeitar aqui
        return false;
    };

    default: return false;
    }
}

// bool constDeclaration(pNode *result, Symbols *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
//     if (*index >= ctx->size()) return false;

//     // Verificar se começa com 'const'
//     Token constToken = std::get<Token>((*ctx)[*index].content);
//     if (constToken.kind != CONST) return false;

//     // Próximo token deve ser o tipo
//     if (*index + 1 >= ctx->size() || (*ctx)[*index + 1].kind != Common) return false;
//     Token typeToken = std::get<Token>((*ctx)[*index + 1].content);

//     Symbol type = Symbol();
//     if (!sym->contains(typeToken.lexeme)) return false;
//     if ((type = sym->get(typeToken.lexeme)).kind != TYPE) return false;

//     // Próximo deve ser o identificador
//     if (*index + 2 >= ctx->size() || (*ctx)[*index + 2].kind != Common) return false;
//     Token identifierToken = std::get<Token>((*ctx)[*index + 2].content);
//     if (identifierToken.kind != IDENTIFIER) return false;
//     if (identifierToken.lexeme.empty()) return false;

//     // Próximo deve ser = ou :=
//     if (*index + 3 >= ctx->size() || (*ctx)[*index + 3].kind != Common) return false;
//     Token symbolToken = std::get<Token>((*ctx)[*index + 3].content);

//     if (symbolToken.kind == EQUAL || symbolToken.kind == COLON_EQUAL) {
//         // Marcar o símbolo como const
//         type.isConst = true;

//         // Adicionar constante à tabela de símbolos durante o parsing
//         Symbol constSymbol(identifierToken.lexeme, CONSTANT, VAR_LOCAL, type.radical, type.bytes);
//         constSymbol.isConst = true;
//         sym->add(identifierToken.lexeme, constSymbol);

//         *result = pNode(dHopefull, type, identifierToken.lexeme);
//         *index += 3; // Consumir const, tipo, identifier, e símbolo

//         return true;
//     }

//     return false;
// }

// bool letDeclaration(pNode *result, Symbols *sym, long unsigned int *index, const std::vector<pContext>* ctx) {
//     if (*index >= ctx->size()) return false;

//     // Verificar se começa com 'let'
//     Token letToken = std::get<Token>((*ctx)[*index].content);
//     if (letToken.kind != LET) return false;

//     (*index)++; // Consumir 'let'

//     bool isMutable = false;

//     // Verificar se próximo token é 'mut'
//     if (*index < ctx->size() && (*ctx)[*index].kind == Common) {
//         Token nextToken = std::get<Token>((*ctx)[*index].content);
//         if (nextToken.kind == MUT) {
//             isMutable = true;
//             (*index)++; // Consumir 'mut'
//         }
//     }

//     // Próximo deve ser o identificador
//     if (*index >= ctx->size() || (*ctx)[*index].kind != Common) return false;
//     Token identifierToken = std::get<Token>((*ctx)[*index].content);
//     if (identifierToken.kind != IDENTIFIER) return false;
//     if (identifierToken.lexeme.empty()) return false;

//     (*index)++; // Consumir identifier

//     // Próximo deve ser :=
//     if (*index >= ctx->size() || (*ctx)[*index].kind != Common) return false;
//     Token symbolToken = std::get<Token>((*ctx)[*index].content);

//     if (symbolToken.kind == COLON_EQUAL) {
//         (*index)++; // Consumir :=

//         // Agora devemos ter uma expressão após :=
//         // Por simplicidade, vamos assumir que é um literal ou expressão simples
//         if (*index >= ctx->size()) return false;

//         const pContext& exprContext = (*ctx)[*index];
//         if (exprContext.kind != Common || !std::holds_alternative<Token>(exprContext.content)) return false;

//         Token exprToken = std::get<Token>(exprContext.content);

//         // Usar o tipo "int" do sistema (equivalente a usize em Rust/Zig)
//         Symbol intType = sym->get("int");
//         Symbol varSymbol(identifierToken.lexeme, VARIABLE, VAR_LOCAL, intType.radical, intType.bytes);
//         varSymbol.isConst = !isMutable; // let é imutável, let mut é mutável

//         // Adicionar à tabela de símbolos durante o parsing
//         sym->add(identifierToken.lexeme, varSymbol);

//         *result = pNode(dHopefull, varSymbol, identifierToken.lexeme);
//         (*index)++; // Consumir expressão

//         // Consumir ';' se presente
//         if (*index < ctx->size() && (*ctx)[*index].kind == Common &&
//             std::holds_alternative<Token>((*ctx)[*index].content) &&
//             std::get<Token>((*ctx)[*index].content).kind == SEMICOLON) {
//             (*index)++; // Consumir ';'
//         }

//         return true;
//     }

//     return false;
// }
