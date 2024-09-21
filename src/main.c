#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "debug.h"
#include "vector.h"
#include "parser.h"
#include "smart.h"
#include "llvm.h"

#define GET(type, root, i) ((type*) root.items)[i]

int
main() 
{
  // Open the source file for reading
  FILE *main = fopen ("./bootstrapping/main.cl", "r+");
  Vector tokens = vector_init (sizeof (Token));
  tokenize (main, &tokens);
  fclose (main);
  
  int i = 0;
  #if DBG_LEXER 
    pHeader (&i, "----- Tokens print: -----");
    pTokens (&tokens);
  #endif

  // Initialize a vector for the parse tree nodes
  Vector root = vector_init (sizeof (PNode));
  pGenerate (&root, &tokens);
  #if DBG_PARSER
    pHeader (&i, "----- Tree print: -----");
    pRoot (&root);
  #endif

  // Initialize a vector for the smart parse tree nodes
  Vector smartRoot = vector_init(sizeof (SNode));
  sGenerate (&smartRoot, &root);

  #if DBG_SMART
    pHeader (&i, "----- Smart print -----");
    pSmart (&smartRoot, &root);
  #endif

   // Open the output file for writing the generated LLVM code
  FILE *output = fopen ("./bootstrapping/output.ll", "w+");
  llGenerate (output, &smartRoot, &root);  
  fclose (output);


  // Clean up heap storage to avoid memory leaks
  // Clean up the tokens vector to free allocated memory for token buffers
  for( int i = 0; i < tokens.length; i++ )
    /*->*/ free (GET(Token, tokens, i).buffer);
  // Free the tokens vector itself
  vector_free (&tokens); 

  // Clean up the parse tree (root) vector
  for( int i = 0; i < root.length; i++)
    {
      PNode node = GET(PNode, root, i);
      if( node.type == Magic )
        /*->*/ free (node.saves.magic);
      if( node.type == LiteralType )
        /*->*/ free (node.saves.lit);
      else
      if( node.type == Definition )
        {
          // Free array data memory for definitions
          free (node.saves.definition.array.size);
          free (node.saves.definition.array.type);
          
          // Free memory for definition metadata
          free (node.saves.definition.id);
          free (node.saves.definition.type);
        }
      else
      if( node.type == ArrayType )
        {
          // Free array-related memory
          free (node.saves.array.size);
          free (node.saves.array.type);
        }
    }
  // Free the parse tree vector itself
  vector_free (&root);

  // Clean up the smart parse tree (smartRoot) vector
  for( int i = 0; i < smartRoot.length; i++) 
    {
      SNode node = GET(SNode, smartRoot, i); 
      if( node.type == DLambda ) 
        /*->*/ free (node.what.lambda.args);
      else
      if( node.type == SDefinition ) 
        {
          // Free array data memory for definitions
          free (node.what.definition.array.size);
          free (node.what.definition.array.type);
          
          // Free memory for definition metadata
          free (node.what.definition.id);
          free (node.what.definition.type);
        }
    }

  // Compile the generated LLVM code based on the operating system
# ifdef _WIN32 
  system ("cd bootstrapping & clang output.ll -o output.exe & cd ..");
# else
  system ("cd bootstrapping & clang output.ll -o output & cd ..");
# endif

  return 0;
}