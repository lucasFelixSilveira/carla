#ifndef STD_H
# define STD_H
# include "parser.h"

  typedef struct {
    char *type;
    char *lib;
    char *args;
    char *id;
  } Fn;

  typedef struct {
    Fn fn;
    char *id;
    char *lib;
  } StackFn;

  Fn std_fn(PNode node);

#endif