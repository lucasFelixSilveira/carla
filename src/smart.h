#ifndef SMART_H
# define SMART_H
# define DBG_SMART 1
# include <stdio.h>
# include "vector.h"
# include "parser.h"

  typedef enum {
    DLambda,
    SDefinition,
    SExpression,
    CloseScope,
    SMagic
  } SmartNodes;

  typedef struct {
    PNode    *id;
    int      *args;
    DMemory   definition;
    int       lArgs;
  } SLambda;

  typedef struct {
    Vector *vec;
    int semantic;
  } Expr;

  typedef union {
    DMemory definition;
    Expr    expr;
    void   *nothing;
    SLambda lambda;
    char   *magic;
  } Declaration;

  typedef struct {
    SmartNodes type;

    /* Definitions */
    Declaration what;
  } SNode;

  typedef enum {
    EXPR_IDENTIFIER_T,
    EXPR_UNDEFINED_T,
    EXPR_COMPARATION,
    EXPR_ARITHMETIC,
    EXPR_INT_T
  } EXPR_T;

  typedef enum {
    EXPR_UNDEFINED_R,
    EXPR_BOOL_R,
    EXPR_INT_R
  } EXPR_R;

  void sGenerate (Vector *sRoot, Vector *pRoot);

#endif