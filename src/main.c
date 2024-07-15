#include <stdio.h>
#include "lexer.h"
#include "vector.h"

const char *bootstrapping = "../bootstrapping/main.cl";
const char *tests = "../bootstrapping/tests/test.cl";

int
main() 
{
  FILE *main = fopen (tests, "rb");

  Vector tokens = vector_init (sizeof (Token));
  tokenize (main, &tokens);

  fclose (main);
  return 0;
}