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
    CloseScope
  } SmartNodes;

  typedef struct {
    PNode    *id;
    int      *args;
    DMemory   definition;
    int       lArgs;
  } SLambda;

  typedef union {
    DMemory definition;
    void   *nothing;
    SLambda lambda;
    Vector *expr;
  } Declaration;

  typedef struct {
    SmartNodes type;

    /* Definitions */
    Declaration what;
  } SNode;

  typedef enum {
    EXPR_IDENTIFIER_T,
    EXPR_UNDEFINED_T,
    EXPR_ARITHMETIC,
    EXPR_INT_T
  } EXPR_T;

  void sGenerate (Vector *sRoot, Vector *pRoot);

#endif