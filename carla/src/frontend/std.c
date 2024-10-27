#include <stdio.h>
#include "parser.h"
#include "std.h"

#define FUNCTIONS_LENTGTH 1
StackFn functions[FUNCTIONS_LENTGTH] = {
  {
    .lib = "stdio",
    .id  = "println",
    .fn  = {
      .libc = "puts",
      .type = "void",
      .lib = "stdio",
      .id  = "println"
    }
  }
};

Fn 
std_fn(PNode node)
{
  Fn result;
  
  int i = 0;
  for(; i < FUNCTIONS_LENTGTH; i++ )
    {
      if(
           strcmp (node.data.internal.function, functions[i].id) != 0
        || strcmp (node.data.internal.lib, functions[i].lib)     != 0
      ) continue;

      result = functions[i].fn;
    }

  return result;
}