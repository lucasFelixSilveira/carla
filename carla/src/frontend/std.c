#include <stdio.h>
#include "parser.h"
#include "std.h"

#define FUNCTIONS_LENTGTH 2
StackFn functions[FUNCTIONS_LENTGTH] = {
  {
    .lib = "stdio",
    .id  = "println",
    .fn  = {
      .type = "void",
      .lib  = "stdio",
      .id   = "println",
      .args = "[]byte"
    }
  },
  {
    .lib = "stdheap",
    .id  = "alloc",
    .fn  = {
      .type = "ptr",
      .lib  = "heap",
      .id   = "alloc",
      .args = "int64"
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
      if( strcmp (node.data.internal.function, functions[i].id) != 0 ) 
        continue;

      if( strcmp (node.data.internal.lib, functions[i].lib) != 0 )
        continue;

      result = functions[i].fn;
    }

  return result;
}