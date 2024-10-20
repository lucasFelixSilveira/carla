#include <stdio.h>
#include <stdlib.h>
#include "utils/carla.h"
#include "utils/vector.h"
#include "debug/debug.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "frontend/llvm.h"
#include "frontend/types.h"

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
  char *output_file;
  carla_project (&output_file);

  // Tokenize the file received
  Vector lexemes = vector_init (sizeof (Token));
  FILE *main = fopen (main_file, "r");
  free (main_file);
  tkGenerate (&lexemes, main);
  fclose (main);
  
  Vector tokens = vector_init (sizeof (Token));
  parseType (&lexemes, &tokens);
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

  // Generate the LLVM code and put it in the output file
  FILE *output = fopen (output_file, "w+");
  free (output_file);
  llGenerate (output, &tree);

  // Close the output file
  fclose (output);

  // Free the buffer memory
  int cl = 0;
  for(; cl < lexemes.length; cl++ )
    free (GET(Token, lexemes, cl).buffer);
  
  cl = 0;
  for(; cl < tokens.length; cl++ )
    free (GET(Token, tokens, cl).buffer);

  vector_free (&lexemes);
  vector_free (&tokens);
  vector_free (&tree);
  return 0;
}