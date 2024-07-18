#include "vector.h"
#include "lexer.h"
#ifndef PARSER_H
# define PARSER_H

  typedef enum { 
    Global, 
    Internal
  } Scope;

  typedef enum {
    Definition
  } NType;

  typedef struct {
    char *type;
    char *id;
  } DMemory;

  typedef union {
    DMemory definition;  
  } Cache;

  typedef struct {
    NType type;
    Scope scope;
    Cache saves;
  } PNode;

  void generate (Vector *root, Vector *tks);

#endif