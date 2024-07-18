#ifndef LEXER_H
# define LEXER_H
# define DBG_LEXER 0
# include <stdio.h>
# include "vector.h"
# define OP_CONSTANT 69
# define LARGE_CONSTANT 00000

  typedef enum {
    Unknown,
    Literal,
    Semi,
    Collon,
    EndOfFile,
    Identifier,
    Keyword,
    Operator,
    RightShift = OP_CONSTANT,
    LeftShift,
    Walrus,
    Inc, Dec,
    Mul, Div, Sub, Add, 
    Imul, Idiv,
    Eq, Ne, Le, Ge
  } TokenType;

  typedef TokenType ArithmeticOperator;
  typedef TokenType OperationOperator;

  typedef struct {
    TokenType type;
    char *buffer;
  } Token;

  void tokenize (FILE *file, Vector *tks);
#endif