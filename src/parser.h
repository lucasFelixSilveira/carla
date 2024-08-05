#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 1
# include "vector.h"
# include "lexer.h"

  typedef struct {
    char **types;
    int length;
  } StackType;

  typedef enum { 
    Global, 
    Internal
  } Scope;

  typedef enum {
    Definition,
    Expression,
    ArrayType,
    Lambda,
    Normal,
    Begin,
    Magic,
    End
  } NType;

  typedef enum {
    ENone,
    ELit,
    ESum
  } expr_t;

  typedef struct {
    char *size;
    char *type;
  } AMemory;

  typedef struct {
    AMemory array;
    char hopeful;
    char *type;
    char *id;
    char arg;
    char key_type;
  } DMemory;

  typedef union {
    DMemory definition;
    AMemory array;
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