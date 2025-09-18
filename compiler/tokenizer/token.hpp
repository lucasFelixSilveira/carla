#pragma once

#include <sstream>
#include <string>
#include "token_kind.hpp"

typedef struct Token {
  public:
    TokenKind kind;
    std::string lexeme;
    int line;

    static struct Token build(TokenKind kind, std::string lexeme, int line);
    std::string to_string();
} Token;

Token
Token::build(TokenKind kind, std::string lexeme, int line) {
    return (Token) { kind, lexeme, line };
}

std::string
Token::to_string() {
    std::stringstream stream;
    stream << "Token { buffer: `" << this->lexeme << "` kind: " << this->kind << " }";
    return stream.str();
}
