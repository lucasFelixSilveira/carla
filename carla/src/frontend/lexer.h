#ifndef LEXER_H
# define LEXER_H
# define DBG_LEXER 1
# include <stdio.h>
# include "../utils/vector.h"

  typedef enum {
    _NO__,
    ComparationOP,
    MethodAccess,
    Identifier,
    EnumType,
    BinaryOP,
    Keyword,
    Integer,
    BitsOP,
    MathOP,
    Macro,
    Quad,
    Semi,
    Text,
    Type,
    Iter,
    End,
    Unknown
  } TokenType;
  
  typedef struct {
    int posV; // Inx
    int posX; // Row
    int posY; // Col
  } TokenLocation;

  typedef struct {
    char *buffer;
    char *def;
    char *sub;
    TokenType type;
    size_t real;
    TokenLocation local;
  } Token;

  void tkGenerate(Vector *lex, FILE *file);

#endif