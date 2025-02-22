#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 1
# include "../utils/vector.h"
# include "./lexer.h"

  typedef struct {
    char *identifier;
  } CacheStructs;

  typedef enum {
    NODE_NUMBER,
    NODE_ID,
    NODE_TEXT,
    NODE_OPERATION,
    NODE_INTERNAL,
    NODE_INTERNAL_SUPER,
    NODE_DEFINITION,
    NODE_DEF_LIBC,
    NODE_SINGLE,
    NODE_ACCESS,
    NODE_LAMBDA,
    NODE_BEGIN,
    NODE_BEGIN_IMPLMENT,
    NODE_TYPE,
    NODE_FOR,
    NODE_IF,
    NODE_ELSE,
    NODE_OUR,
    NODE_STRUCT,
    NODE_CLOSE,
    NODE_RET,
    NODE_END,
    NODE_END_IMPLEMENT,
    NODE_INTERNAL_STRUCT,
    NODE_FIELD_ACCESS,
    NODE_ASSIGNMENT,
    NODE_EEXPR
  } NodeType;

  typedef union {
    int   number;
    char *value;
    struct {
      char  hopeful;
      char *id;
      char *type;
    } definition;
    struct {
      struct Node *left; 
      struct Node *right; 
    } operation;
    struct Node *single;
  } uNode;

  typedef struct PNode {
    NodeType type;
    union {
      char  our;
      int   number;
      char *value;
      struct {
        char  hopeful;
        char  iter;
        char *id;
        char *type;
      } definition;
      struct {
        Token left; 
        Token operation; 
        Token right; 
      } operation;
      struct {
        union {
          int   number;
          char *value;
        } data;
        NodeType type;
      } single;
      struct {
        char *function;
        char *lib;
      } internal;
      struct {
        char *__struct;
        char *fn;
      } internal_struct;
      char *super;
    } data;
  } PNode;

  void tGenerate(Vector *tree, Vector *tks, Vector *libs);
#endif