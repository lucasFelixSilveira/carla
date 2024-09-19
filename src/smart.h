#ifndef SMART_H
# define SMART_H
# define DBG_SMART 1
# include <stdio.h>
# include "vector.h"
# include "parser.h"

  typedef enum {
    DLambda,
    SDefinition,
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
  } Declaration;

  typedef struct {
    SmartNodes type;

    /* Definitions */
    Declaration what;
  } SNode;

  void sGenerate (Vector *sRoot, Vector *pRoot);

#endif