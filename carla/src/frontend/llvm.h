#ifndef LLVM_H
# define LLVM_H
# include "../utils/vector.h"
# include "std.h"

  typedef enum {
    Lambda,
    If,
    Else,
    For
  } ScopeType;

  typedef enum {
    Normal,
    Function,
    Callback
  } VarType;

  typedef struct {
    VarType v_type;
    int tab;
    unsigned int llvm;
    char *type;
    char *id;
  } Variable;

  typedef enum {
    ACCESS_EXPR,
    FUNCTION_CALL
  } eExprCache;

  typedef struct {
    eExprCache type;
    union {
      struct {
        unsigned int load_vec;
        char *type;
        char *id;
      } access;
      Fn fn_call;
    } info;
  } ExprCache;

  void llGenerate(FILE *output, Vector *pTree);

#endif