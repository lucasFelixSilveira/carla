// Isto le os lexemas contidos dentro do codigo Carla

#pragma once

#include "token.hpp"
#include "token_kind.hpp"
#include <cctype>
#include <ios>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Scanner {
private:
    static void addBuffer(std::vector<Token> *vec, TokenKind kind, std::string buff);
    static void addSimple(std::vector<Token> *vec, TokenKind kind);
public:
    static std::vector<Token> read(std::vector<char> source, std::streamsize size);
};

int line = 1;

std::vector<Token>
Scanner::read(std::vector<char> source, std::streamsize size)
{
    std::map<std::string, TokenKind> keywords = {
        {"return", RETURN},
        {"break", BREAK},
        {"continue", CONTINUE},

        {"struct", STRUCT},
        {"enum", ENUM},

        {"false", FALSE},
        {"true", TRUE},
        {"nil", NIL},

        {"our", OUR},
        {"const", CONST},
        {"if", IF},
        {"else", ELSE},
        {"sizeof", SIZEOF},
        {"let", LET},
        {"mut", MUT},
        {"return", RETURN},

        {"@type", TYPE},
        {"@use", USE},
        {"@import", IMPORT}
    };

    std::vector<Token> tokens;

    std::stringstream buffer;
    const char *str = source.data();
    int i = 0;
    for(; i < size; i++) {
        char ch = str[i];

        switch(ch) {
            case ',': addSimple(&tokens, COMMA); break;

            case '*': addSimple(&tokens, STAR); break;
            case '/': addSimple(&tokens, SLASH); break;
            case ';': addSimple(&tokens, SEMICOLON); break;

            case '(': addSimple(&tokens, LEFT_PAREN); break;
            case ')': addSimple(&tokens, RIGHT_PAREN); break;
            case '[': addSimple(&tokens, LEFT_BRACE); break;
            case ']': addSimple(&tokens, RIGHT_BRACE); break;
            case '{': addSimple(&tokens, LEFT_BRACKET); break;
            case '}': addSimple(&tokens, RIGHT_BRACKET); break;

            case '+':
            if( str[++i] == '-' ) addSimple(&tokens, PLUS_PLUS);
            else { i--; addSimple(&tokens, PLUS); }
            break;

            case ':':
            if( str[++i] == ':' ) addSimple(&tokens, QUAD);
            else if( str[i] == '=' ) addSimple(&tokens, COLON_EQUAL);
            else { i--; addSimple(&tokens, COLON); }
            break;

            case '-':
            if( str[++i] == '-' ) addSimple(&tokens, MINUS_MINUS);
            else if( str[++i] == '>' ) addSimple(&tokens, ARROW);
            else { i--; addSimple(&tokens, MINUS); }
            break;

            case '>':
            if( str[++i] == '=' ) addSimple(&tokens, GREATER_EQUAL);
            else if( str[i] == '>' ) addSimple(&tokens, GREATER_GREATER);
            else { i--; addSimple(&tokens, GREATER); }
            break;

            case '<':
            if( str[++i] == '=' ) addSimple(&tokens, LESS_EQUAL);
            else if( str[i] == '<' ) addSimple(&tokens, LESS_LESS);
            else { i--; addSimple(&tokens, LESS); }
            break;

            case '.':
            if( str[++i] == '.' ) addSimple(&tokens, ITERABLE);
            else { i--; addSimple(&tokens, DOT); }
            break;

            case '=':
            if( str[++i] == '=' ) addSimple(&tokens, EQUAL_EQUAL);
            else { i--; addSimple(&tokens, EQUAL); }
            break;

            case '!':
            if( str[++i] == '=' ) addSimple(&tokens, BANG_EQUAL);
            else { i--; addSimple(&tokens, BANG); }
            break;

            case '\r':
            case '\t':
            case ' ':
            continue;

            case '\n':
            line++;
            break;

            case '"':
            case '\'': {
            buffer << ch;
            char c2, backup = '\0';
            while(i < size) {
                c2 = str[++i];
                buffer << c2;
                if( (( c2 == '\'' && ch == '\'' ) || ( c2 == '"'  && ch == '"' )) && backup != '\\' ) break;
                backup = c2;
            }
            addBuffer(&tokens, STRING, buffer.str());
            buffer.clear();
            buffer.str("");
            } break;

            case '@':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_': {
                buffer << ch;

                while(i + 1 < size) {
                    char c2 = str[i + 1];
                    if (std::isalnum(c2) || c2 == '_') {
                        buffer << c2;
                        i++;
                    } else {
                        break;
                    }
                }

                if( i + 1 < size && str[i + 1] == '!') {
                    buffer << '!';
                    i++;
                }

                std::string tokenStr = buffer.str();
                auto it = keywords.find(tokenStr);
                if( it != keywords.end() ) addSimple(&tokens, it->second);
                else addBuffer(&tokens, IDENTIFIER, tokenStr);

                buffer.str("");
                buffer.clear();
                break;
            }

            case '0'... '9':
            buffer << ch;
            bool isfloat = false;

            while(i + 1 < size) {
                char c2 = str[i + 1];
                if( std::isdigit(c2) ) {
                    buffer << c2;
                    i++;
                }
                else if (c2 == '.' && !isfloat) {
                    buffer << c2;
                    isfloat = true;
                    i++;
                }
                else break;
            }

            addBuffer(&tokens, NUMBER, buffer.str());

            buffer.str("");
            buffer.clear();

            break;
        }
    }

    addSimple(&tokens, CARLA_EOF);
    return tokens;
}

void
Scanner::addSimple(std::vector<Token> *vec, TokenKind kind)
{ (*vec).push_back(Token::build(kind, "", line)); }

void
Scanner::addBuffer(std::vector<Token> *vec, TokenKind kind, std::string buff)
{ (*vec).push_back(Token::build(kind, buff, line)); }
