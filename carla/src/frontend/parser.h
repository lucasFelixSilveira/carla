#ifndef PARSER_H
# define PARSER_H
# define DBG_PARSER 1
# include "../utils/vector.h"
# include "./lexer.h"

  typedef struct {
    char *identifier;
    char *_type;
  } CacheStructs;

  typedef struct {
    char *identifier;
  } EnumFieldName;

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
    NODE_METHOD,
    NODE_ENUM,
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
    TokenLocation posD;
    NodeType type;
    union {
      char  our;
      int   number;
      char *value;
      struct {
        char  hopeful;
        char  is_bound;
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
        char *instance_id;
        char *__struct;
        char *fn;
        char need_pass_the_instance;
      } internal_struct;
      char *super;
      struct {
        struct {
          char *integer_t; 
          char *definition; 
        } ctx;
        Vector *pFields;
      } enumerator;
    } data;
  } PNode;

  void tGenerate(Vector *tree, Vector *tks, Vector *libs);
#endif