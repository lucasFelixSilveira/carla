#ifndef LEXER_H
# define LEXER_H
# define DBG_LEXER 1
# include <stdio.h>
# include "../utils/vector.h"

  typedef enum {
    _NO__,
    ComparationOP,
    Identifier,
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
    char *buffer;
    TokenType type;
    size_t real;
  } Token;

  void tkGenerate(Vector *lex, FILE *file);

#endif