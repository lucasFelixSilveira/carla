#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
#include "../parser/ast.hpp"
#include "../parser/parser.hpp"
#include "../parser/symbols.hpp"

#include "./gen.hpp"

std::string generateMorganaCode(std::vector<pNode> nodes, Symbols symbols, bool func);

int var = 0;

std::vector<int> stack_index;
std::vector<int>  func_index;

struct morg {
    static std::string toStr(Symbol type);

    static std::string lambda(Symbols& sym, pNode declaration, pNode lambda);
};

std::string morg::toStr(Symbol type) {
    if( type.kind == TYPE ) {
        if( type.radical == i8 )        return "i8";
        else if( type.radical == i16 )  return "i16";
        else if( type.radical == i32 )  return "i32";
        else if( type.radical == i64 )  return "i64";
        else if( type.radical == i128 ) return "i128";
        else if( type.radical == i256 ) return "i256";
        else return "ptr";
    } else return "void";
}

std::string morg::lambda(Symbols& sym, pNode declaration, pNode lambda) {
    var = 0;

    std::stringstream ss;

    pDeclaration decl = std::get<pDeclaration>(declaration.values);
    pLambda func = std::get<pLambda>(lambda.values);
    std::string type = toStr(decl.type);

    ss << type << ' ' << decl.name << ' ';

    bool first = true;
    for( auto param : func.args ) {
        pDeclaration arg = std::get<pDeclaration>(param.values);
        ss << toStr(arg.type) << " ";
        first = false;
    }

    var++;

    ss << "{\n";

    if( func.args.size() > 0 ) {
        ss << "(";
        for( auto param : func.args ) {
            pDeclaration arg = std::get<pDeclaration>(param.values);
            ss << arg.name << ", ";
            first = false;
        }
        ss << ") @_\n";
    }

    std::vector<pNode> stmt;
    Parser::checkSyntax(sym, &stmt, std::get<std::vector<pContext>>(func.body.content), false);
    ss << generateMorganaCode(stmt, sym, true);

    ss << "ret 0\n";
    ss << "}\n\n";

    var = 0;

    return ss.str();
}
