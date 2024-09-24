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

  typedef PNode ExprNode;

  typedef union {
    DMemory definition;
    ExprNode    expr;
    void   *nothing;
    SLambda lambda;
    char   *magic;
  } Declaration;

  typedef struct {
    SmartNodes type;

    /* Definitions */
    Declaration what;
  } SNode;

  void sGenerate (Vector *sRoot, Vector *pRoot);

#endif