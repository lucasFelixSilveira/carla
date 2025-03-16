#ifndef LLVM_H
# define LLVM_H
# include "../utils/vector.h"
# include "parser.h"
# include "lexer.h"
# include "std.h"

  typedef enum {
    Lambda,
    If,
    Else,
    For,
    For_complex,
    Else_scope,
    If_scope,
    Elif_scope
  } scope_t;

  typedef struct {
    scope_t type;
    unsigned int label_id;
    struct {
      unsigned int left;
      char *operator;
    } operation;
    struct {
      unsigned int left;
      unsigned int comp;     
      char *type;  
    } complex;
    struct {
      unsigned int _false;  
    } _if;
  } ScopeType;

  typedef struct {
    char *type;
  } RetStack;

  typedef enum {
    Normal,
    Function,
    Callback,
    StructField,
    Enumerator
  } VarType;

  typedef struct {
    VarType v_type;
    TokenLocation local;
    char *__struct;
    int tab;
    unsigned int llvm;
    char *type;
    char *id;
    char public;
    char bounded;
    long struct_i;
  } Variable;

  typedef enum {
    ACCESS_EXPR,
    FUNCTION_CALL,
    RETURN_KEY,
    IF_KEY,
    ELIF_KEY,
    VAR_DECLARATION,
    ASSIGNMENT_FIELD
  } eExprCache;

  typedef struct {
    eExprCache type;
    union {
      unsigned int llvm;
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

  typedef struct {
    char *string;
    int type;
    int id;
  } Complement;

  typedef struct {
    char *struct_id;
    unsigned int carla_id; 
  } Structies;

  void llGenerate(FILE *output, Vector *pTree, FILE *logs);

#endif