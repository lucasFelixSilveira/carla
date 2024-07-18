#include <stdio.h>
#include "lexer.h"
#include "debug.h"
#include "vector.h"
#include "parser.h"

const char *bootstrapping = "./bootstrapping/main.cl";
const char *tests = "./bootstrapping/tests/test.cl";
int i = 0;

int
main() 
{
  FILE *main = fopen (bootstrapping, "r+");

  Vector tokens = vector_init (sizeof (Token));
  tokenize (main, &tokens);
  Vector sym_table = vector_init (1);

  #if DBG_LEXER 
    pHeader (&i, "----- Tokens print: -----");
    pTokens (&tokens);
  #endif

  Vector root = vector_init(sizeof (PNode));
  generate (&root, &tokens);
  #if DBG_PARSER
    pHeader (&i, "----- Tree print: -----");
    pRoot (&root);
  #endif

  fclose (main);
  return 2;
}