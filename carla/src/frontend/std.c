#include <stdio.h>
#include "parser.h"
#include "std.h"

#define FUNCTIONS_LENTGTH 6
StackFn functions[FUNCTIONS_LENTGTH] = {
  {
    .lib = "stdio",
    .id  = "println",
    .fn  = {
      .type = "int32",
      .lib  = "stdio",
      .id   = "println",
      .args = "[]byte"
    }
  },
  {
    .lib = "stdio",
    .id  = "print",
    .fn  = {
      .type = "int32",
      .lib  = "stdio",
      .id   = "print",
      .args = "[]byte"
    }
  },
  {
    .lib = "stdio",
    .id  = "putc",
    .fn  = {
      .type = "int32",
      .lib  = "stdio",
      .id   = "putc",
      .args = "i8"
    }
  },
  {
    .lib = "stdio",
    .id  = "puti",
    .fn  = {
      .type = "int32",
      .lib  = "stdio",
      .id   = "puti",
      .args = "i32"
    }
  },
  {
    .lib = "stdheap",
    .id  = "alloc",
    .fn  = {
      .type = "[]any",
      .lib  = "heap",
      .id   = "alloc",
      .args = "int64"
    }
  },
  {
    .lib = "stdstring",
    .id  = "format",
    .fn  = {
      .type = "int32",
      .lib  = "string",
      .id   = "format",
      .args = "[]byte []byte ..."
    }
  },
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