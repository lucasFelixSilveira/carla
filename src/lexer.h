#ifndef LEXER_H
# define LEXER_H
# define DBG_LEXER 1
# include <stdio.h>
# include "vector.h"
# define OP_CONSTANT 69
# define COMP_CONSTANT 123
# define LARGE_CONSTANT 10000

  typedef enum {
    Unknown,
    Integer,
    Literal,
    Semi,
    Iter,
    EndOfFile,
    Identifier,
    Keyword,
    Operator,
    ArithmeticOperator,

    /* BITs */
    RightShift = OP_CONSTANT,
    LeftShift,

    /* Arithmetic */
    Inc, Dec,
    Mul, Div, Sub, Add, 
    Imul, Idiv,

    /* Comparation */
    Eq = COMP_CONSTANT, Ne, Le, Ge, LL, GG,

    /* Iterators */
    Walrus, Quad, Acess
  } TokenType;

  typedef TokenType OperationOperator;

  typedef struct {
    TokenType type;
    char *buffer;
    size_t real;
  } Token;

  void tokenize (FILE *file, Vector *tks);
#endif