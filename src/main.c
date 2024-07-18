#include <stdio.h>
#include "lexer.h"
#include "vector.h"
#include "parser.h"

#define DEBUG 1

const char *bootstrapping = "./bootstrapping/main.cl";
const char *tests = "./bootstrapping/tests/test.cl";

int
main() 
{
  FILE *main = fopen (bootstrapping, "r+");

  Vector tokens = vector_init (sizeof (Token));
  tokenize (main, &tokens);

  #if DEBUG 
    for(
      int i = 0;
      i < tokens.length; 
      i++
    ) {
        Token tk = ((Token*)tokens.items)[i];
        printf("Token<%d> {\n\tbuffer: \"%s\",\n\ttype: %d\n},\n", i, tk.buffer, tk.type);
      }
  #endif

  generate (&tokens);
  fclose (main);
  return 2;
}