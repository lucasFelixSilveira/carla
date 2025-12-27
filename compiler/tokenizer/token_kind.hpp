// Isto define os tipos de tokens possiveis de se ter em carla
#pragma once

#include <string>
typedef enum TokenKind {
    LEFT_PAREN,   RIGHT_PAREN,
    LEFT_BRACE,   RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,

    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, PLUS_PLUS, MINUS_MINUS, QUAD,

    ITERABLE, COLON, COLON_EQUAL,

    ARROW,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL, GREATER_GREATER,
    LESS, LESS_EQUAL, LESS_LESS,

    IDENTIFIER, STRING, NUMBER, INCLUDE,

    AND, OR, NIL, FALSE, TRUE,

    RETURN, AUTO, OUR, FOR, STRUCT, ENUM, BREAK, CONTINUE, IF, CONST, ELSE, SIZEOF, LET, MUT, END_KEYWORDS,

    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,

    TYPE, USE, IMPORT,

    CARLA_EOF,
    UNKNOWN_TK
} TokenKind;

typedef enum TokenSubKind {
    OPEN_CONTEXT, CLOSE_CONTEXT,

    UNKNOWN
} TokenSubKind;

TokenSubKind getSub(TokenKind kind) {
    switch(kind) {
        case LEFT_PAREN:
        case LEFT_BRACE:
        case LEFT_BRACKET: return OPEN_CONTEXT;

        case RIGHT_BRACE:
        case RIGHT_PAREN:
        case RIGHT_BRACKET: return CLOSE_CONTEXT;

        default: return UNKNOWN;
    }
}

std::string tokenKindToString(TokenKind kind) {
    switch (kind) {
        case LEFT_PAREN:        return "LEFT_PAREN";
        case RIGHT_PAREN:       return "RIGHT_PAREN";
        case LEFT_BRACE:        return "LEFT_BRACE";
        case RIGHT_BRACE:       return "RIGHT_BRACE";
        case LEFT_BRACKET:      return "LEFT_BRACKET";
        case RIGHT_BRACKET:     return "RIGHT_BRACKET";

        case COMMA:             return "COMMA";
        case DOT:               return "DOT";
        case MINUS:             return "MINUS";
        case PLUS:              return "PLUS";
        case SEMICOLON:         return "SEMICOLON";
        case SLASH:             return "SLASH";
        case STAR:              return "STAR";
        case PLUS_PLUS:         return "PLUS_PLUS";
        case MINUS_MINUS:       return "MINUS_MINUS";
        case QUAD:              return "QUAD";

        case ITERABLE:          return "ITERABLE";
        case COLON:             return "COLON";
        case COLON_EQUAL:       return "COLON_EQUAL";

        case ARROW:             return "ARROW";

        case BANG:              return "BANG";
        case BANG_EQUAL:        return "BANG_EQUAL";
        case EQUAL:             return "EQUAL";
        case EQUAL_EQUAL:       return "EQUAL_EQUAL";
        case GREATER:           return "GREATER";
        case GREATER_EQUAL:     return "GREATER_EQUAL";
        case GREATER_GREATER:   return "GREATER_GREATER";
        case LESS:              return "LESS";
        case LESS_EQUAL:        return "LESS_EQUAL";
        case LESS_LESS:         return "LESS_LESS";

        case IDENTIFIER:        return "IDENTIFIER";
        case STRING:            return "STRING";
        case NUMBER:            return "NUMBER";
        case INCLUDE:           return "INCLUDE";

        case AND:               return "AND";
        case OR:                return "OR";
        case NIL:               return "NIL";
        case FALSE:             return "FALSE";
        case TRUE:              return "TRUE";

        case RETURN:            return "RETURN";
        case AUTO:              return "AUTO";
        case OUR:               return "OUR";
        case FOR:               return "FOR";
        case STRUCT:            return "STRUCT";
        case ENUM:              return "ENUM";
        case BREAK:             return "BREAK";
        case CONTINUE:          return "CONTINUE";
        case IF:                return "IF";
        case CONST:             return "CONST";
        case ELSE:              return "ELSE";
        case SIZEOF:            return "SIZEOF";
        case LET:               return "LET";
        case MUT:               return "MUT";

        case PLUS_EQUAL:        return "PLUS_EQUAL";
        case MINUS_EQUAL:       return "MINUS_EQUAL";
        case STAR_EQUAL:        return "STAR_EQUAL";
        case SLASH_EQUAL:       return "SLASH_EQUAL";

        case TYPE:              return "TYPE";
        case USE:               return "USE";
        case IMPORT:            return "IMPORT";

        case CARLA_EOF:         return "CARLA_EOF";

        default:                return "UNKNOWN";
    }
}
