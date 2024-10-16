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
    NODE_LAMBDA,
    NODE_BEGIN,
    NODE_END
  } NodeType;

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
        struct Node *left; 
        struct Node *right; 
      } op;
    } data;
  } PNode;

  void tGenerate(Vector *tree, Vector *tks);
#endif