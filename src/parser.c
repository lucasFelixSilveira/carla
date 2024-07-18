#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "vector.h"

typedef enum { false = 0, true = 1 } bool;

void 
generate (Vector *tks) 
{
  int i = 0;
  while( i < tks->length ) 
    {
      Token tk = ((Token*)tks->items)[i++];

      /*
        Identifier:
          -> Identifier(Variable) + Keyword(as) + Identifier(Type)
          -> Identifier(Variable) + Operator(.) + Expr
          -> Identifier(Variable) + Operator(%+-*)
      */
      if( tk.type == Identifier ) {

      }
    }
}