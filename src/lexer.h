#include <stdio.h>
#ifndef LEXER_H
# include "vector.h"
# define LEXER_H

typedef enum {
  Identifier,
  Keyword,
  Operator,
  BinaryOperator,
  Walrus,
  Sign,
  Collon,
  Semi
} TokenType;

typedef struct {
  TokenType type;
  char *buffer;
} Token;

Token *tokenize (FILE *file, Vector *tks);

#endif