#ifndef LLVM_H
# define LLVM_H
# include "../utils/vector.h"
# include "parser.h"
# include "std.h"

  typedef enum {
    Lambda,
    If,
    Else,
    For
  } ScopeType;

  typedef struct {
    char *type;
  } RetStack;

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
    FUNCTION_CALL,
    RETURN_KEY,
    VAR_DECLARATION
  } eExprCache;

  typedef struct {
    eExprCache type;
    union {
      char nothing;
      PNode node;
      struct {
        unsigned int load_vec;
        char *type;
        char *id;
        char need_load;
      } access;
      Fn fn_call;
      struct {
        PNode node;
        unsigned int llvm;
      } var;
    } info;
  } ExprCache;

  void llGenerate(FILE *output, Vector *pTree);

#endif