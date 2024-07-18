#include <stdio.h>
#ifndef LEXER_H
# include "vector.h"
# define LEXER_H
  typedef enum {
    Identifier,
    Keyword,
    Operator,
    RightShift,
    LeftShift,
    Walrus,
    Inc,
    Dec,
    Mul,
    Div,
    Imul,
    Idiv,
    Sub,
    Add,
    Eq,
    Ne,
    Le,
    Ge,
    Sign,
    Collon,
    Semi,
    Unknown,
    EndOfFile
  } TokenType;

  typedef TokenType ArithmeticOperator;
  typedef TokenType OperationOperator;

  typedef struct {
    TokenType type;
    char *buffer;
  } Token;

  void tokenize (FILE *file, Vector *tks);
#endif