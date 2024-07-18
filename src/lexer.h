#include <stdio.h>
#ifndef LEXER_H
# include "vector.h"
# define LEXER_H
# define OP_CONSTANT 69
  typedef enum {
    Unknown,
    Semi,
    Collon,
    EndOfFile,
    Identifier,
    Keyword,
    Operator,
    RightShift = OP_CONSTANT,
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
    Sign
  } TokenType;

  typedef TokenType ArithmeticOperator;
  typedef TokenType OperationOperator;

  typedef struct {
    TokenType type;
    char *buffer;
  } Token;

  void tokenize (FILE *file, Vector *tks);
#endif