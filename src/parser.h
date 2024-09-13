#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 0
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
    LiteralType,
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
    char key_type;
    char hopeful;
    char *type;
    char *id;
    char arg;
  } DMemory;

  typedef union {
    DMemory definition;
    AMemory array;
    char nothing;
    Token token;
    char *magic;
    char *lit;
  } Cache;

  typedef struct {
    Scope scope;
    Cache saves;
    NType type;
  } PNode;

  void pGenerate (Vector *root, Vector *tks);

#endif