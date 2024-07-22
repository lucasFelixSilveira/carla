#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "debug.h"
#include "vector.h"
#include "parser.h"
#include "llvm.h"

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
  pGenerate (&root, &tokens);
  #if DBG_PARSER
    pHeader (&i, "----- Tree print: -----");
    pRoot (&root);
  #endif

  FILE *output = fopen ("./bootstrapping/output.ll", "w+");
  llGenerate (output, &root);

  for( int i = 0; i < tokens.length; i++ )
    /*->*/ free (((Token*) tokens.items)[i].buffer);
  vector_free (&tokens);
  vector_free (&root);

  fclose (output);
  fclose (main);

# ifdef _WIN32 
  system ("cd bootstrapping & clang output.ll -o output.exe & cd ..");
# else
  system ("cd bootstrapping & clang output.ll -o output & cd ..");
# endif

  return 0;
}