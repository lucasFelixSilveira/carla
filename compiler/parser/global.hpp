#pragma once

#include <map>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "ast.hpp"
#include "../utils/result.hpp"
#include "../compiler_outputs.hpp"
#include "./parser.hpp"

inline Result checkSyntax(std::vector<pNode> *nodes,std::vector<pContext>& ctx);

struct GlobalData {
private:
    static pNode last;
    static pNode lastGlobal;

public:
    enum class Kind {
        Lambda
    };

    using SymbolData = std::variant<std::monostate, std::vector<pNode>>;
    using SymbolEntry = std::tuple<Kind, SymbolData>;

    static inline std::map<std::string, SymbolEntry> symbols;
    inline static void setPNode(pNode node) { last = node; }
    inline static pNode getPNode() { return last; }

    inline static void setGPNode(pNode node) { lastGlobal = node; }
    inline static pNode getGPNode() { return lastGlobal; }

    static void addLambdaBody(const pContext& body) {
        std::vector<pNode> parsedNodes;
        auto contexts = std::get<std::vector<pContext>>(body.content);

        Result syntaxResult = checkSyntax(&parsedNodes, contexts);

        if(! isSuccess(syntaxResult) ) CompilerOutputs::Fatal(err(syntaxResult));

        auto decl = std::get<pDeclaration>(lastGlobal.values);
        const std::string name = decl.name;
        symbols.insert({name, { Kind::Lambda, parsedNodes }});
    }
};

pNode GlobalData::last = pNode();
pNode GlobalData::lastGlobal = pNode();
