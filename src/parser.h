#include "vector.h"
#include "lexer.h"
#ifndef PARSER_H
# define PARSER_H

  typedef enum {
    Arithmetic,
    Operation,
    Scope
  } NType;

  typedef struct {
    /*
      exemple: 1 + 2
      expected output: {
        .before = &tk(1),
        .symbol = &tk(+),
        .after = &tk(2),
      }
    */
    Token before;
    ArithmeticOperator symbol;
    Token after;
  } ArithmeticExpr;

  typedef struct {
    /*
      exemple: x++
      expected output: {
        .before &tk(X),
        .symbol = &tk(++),
      }
    */
    Token before;
    OperationOperator symbol;
  } OperationExpr;

  typedef void* ScopeRoot;
  typedef struct {
    /*
      { ... }

      expected output: {
        .nStack = {SyntaxRoot}
      }
    */   
    ScopeRoot *nStack;  
  } ScopeExpr;
  typedef union {
    ScopeExpr scope;
    ArithmeticExpr arithmetic;
    OperationExpr operation;
  } NContext;

  typedef struct Node {
    NContext *context;
    NType type; 
  } Node;

  typedef struct {
    Node *top;
    Node *root;
  } SyntaxRoot;

  void genAST (SyntaxRoot *root, Vector *tks);
  /* TODO! 
  void printTree (SyntaxRoot *root);
  */
#endif