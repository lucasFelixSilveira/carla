#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 1
# include "../utils/vector.h"

  typedef enum {
    NODE_NUMBER,
    NODE_ID,
    NODE_TEXT,
    NODE_OPERATION,
    NODE_DEFINITION,
    NODE_DEF_LIBC,
    NODE_SINGLE,
    NODE_ACCESS,
    NODE_LAMBDA,
    NODE_BEGIN,
    NODE_TYPE,
    NODE_CUT,
    NODE_END
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
      int   number;
      char *value;
      struct {
        char  hopeful;
        char *id;
        char *type;
      } definition;
      struct {
        struct PNode *left; 
        struct PNode *right; 
      } operation;
      struct {
        union {
          int   number;
          char *value;
        } data;
        NodeType type;
      } single;
    } data;
  } PNode;

  void tGenerate(Vector *tree, Vector *tks);
#endif