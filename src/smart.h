#ifndef SMART_H
# define SMART_H
# define DBG_SMART 1
# include <stdio.h>
# include "vector.h"
# include "parser.h"

  typedef enum {
    DLambda,
    SDefinition,
    SExprNode,
    CloseScope,
    SMagic
  } SmartNodes;

  typedef struct {
    PNode    *id;
    int      *args;
    DMemory   definition;
    int       lArgs;
  } SLambda;

  typedef enum {
    SingleExpr,
    CastExpr
  } Sexpr_t;

  typedef union {
    char *single;
    PNode cast;
    // ExprArithmetic arithmetic;
  } expr_v; 

  typedef struct {
    Sexpr_t type;
    expr_v value;
  } Expr;

  typedef union {
    DMemory definition;
    void   *nothing;
    SLambda lambda;
    char   *magic;
    Expr    expr;
  } Declaration;

  typedef struct {
    SmartNodes type;

    /* Definitions */
    Declaration what;
  } SNode;

  void sGenerate (Vector *sRoot, Vector *pRoot);

#endif