#include <stdio.h>
#include "lexer.h"
#include "debug.h"
#include "vector.h"
#include "parser.h"

int
main() 
{
  FILE *main = fopen ("./bootstrapping/main.cl", "r+");
  Vector tokens = vector_init (sizeof (Token));
  tokenize (main, &tokens);
  
  int i = 0;
  #if DBG_LEXER 
    pHeader (&i, "----- Tokens print: -----");
    pTokens (&tokens);
  #endif

  Vector root = vector_init (sizeof (PNode));
  generate (&root, &tokens);
  #if DBG_PARSER
    pHeader (&i, "----- Tree print: -----");
    pRoot (&root);
  #endif

  fclose (main);
  return 2;
}