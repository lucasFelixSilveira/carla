#include <stdio.h>
#include <stdlib.h>
#include "utils/carla.h"
#include "utils/vector.h"
#include "debug/debug.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"

#define GET(type, root, i) ((type*) root.items)[i]

int
main(int argc, char *argv[]) 
{
  if( argc <= 1 ) 
    return 1;
  
  // Check if the received file is a Carla file
  char *main_file = argv[1];
  int carla = isCarla (main_file);
  if(! carla )
    return 1;

  // Tokenize the file received
  Vector tokens = vector_init (sizeof (Token));
  FILE *main = fopen (main_file, "r");
  free (main_file);
  tkGenerate (&tokens, main);
  fclose (main);
  int i = 0;

  #if DBG_LEXER 
    // Debug the TOKENS vector
    pHeader (&i, "----- Tokens print: -----");
    pTokens (&tokens);
  #endif

  // Generate a Tree using the tokens
  Vector tree = vector_init (sizeof (PNode));
  tGenerate (&tree, &tokens);

  #if DBG_PARSER 
    // Debug the PNode vector
    pHeader (&i, "----- Tree print: -----");
    pNodes (&tree);
  #endif

  // Free the buffer memory
  int cl = 0;
  for(; cl < tokens.length; cl++ )
    free (GET(Token, tokens, cl).buffer);
  vector_free (&tokens);
  vector_free (&tree);
  return 0;
}