#ifndef TYPES_H
# define TYPES_H
# include "../utils/vector.h"
# include <stdio.h>
  void parseType(Vector *lex, Vector *tks, FILE *logs);
  char types_int(char *type);
#endif