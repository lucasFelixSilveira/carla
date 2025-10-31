#pragma once

#include <vector>
#include "../parser/ast.hpp"
#include "../parser/parser.hpp"
#include "../parser/symbols.hpp"

#include "./gen.hpp"

std::string generateMorganaCode(std::vector<pNode> nodes, Symbols symbols, bool func);

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
    std::stringstream ss;

    pDeclaration decl = std::get<pDeclaration>(declaration.values);
    pLambda func = std::get<pLambda>(lambda.values);
    std::string type = toStr(decl.type);

    ss << type << ' ' << decl.name << ' ';

    for( auto param : func.args ) {
        pDeclaration arg = std::get<pDeclaration>(param.values);
        ss << toStr(arg.type) << " ";
    }

    ss << "{\n";

    unsigned int len = func.args.size();
    for( int i = 0; i < len; i++ ) {
        if( i == 0  )         ss << "(";
        if( i == (len - 1)  ) ss << ") @_\n";

        auto param = func.args[i];
        pDeclaration arg = std::get<pDeclaration>(param.values);
        ss << arg.name << ", ";
    }

    std::vector<pNode> stmt;
    Parser::checkSyntax(sym, &stmt, std::get<std::vector<pContext>>(func.body.content), false);
    ss << generateMorganaCode(stmt, sym, true);

    ss << "ret 0\n";
    ss << "}\n\n";

    return ss.str();
}
