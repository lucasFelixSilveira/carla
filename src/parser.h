#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 1
# include "vector.h"
# include "lexer.h"

  typedef enum { 
    Global, 
    Internal
  } Scope;

  typedef enum {
    Definition,
    Expression,
    Lambda,
    Normal,
    Begin,
    Magic,
    End
  } NType;

  typedef enum {
    Comparation,
    Arithmetic,
    Operation,
    Binary
  } expr_t;

  typedef struct {
    expr_t type;
    char endded;
    /* before -> operator -> after -> (...)? */
    Token *before;
    char  *operator;
    Token *after;
    char incomplete;
  } _Expression; 

  typedef struct {
    char hopeful;
    char *type;
    char *id;
  } DMemory;

  typedef union {
    _Expression expression;
    DMemory definition;
    char nothing;
    Token token;
    char *magic;
  } Cache;

  typedef struct {
    NType type;
    Scope scope;
    Cache saves;
  } PNode;

  void pGenerate (Vector *root, Vector *tks);

#endif