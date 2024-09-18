#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "debug.h"
#include "vector.h"
#include "parser.h"
#include "smart.h"
#include "llvm.h"

#define TKGET(tks, i) ((Token*) tks.items)[i]
#define BRANCHGET(root, i) ((PNode*) root.items)[i]

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


  for( int i = 0; i < tokens.length; i++ )
    /*->*/ free (TKGET(tokens, i).buffer);
  vector_free (&tokens);

  Vector smartRoot = vector_init(sizeof (SNode));
  sGenerate (&smartRoot, &root);

  #if DBG_SMART
    pHeader (&i, "----- Smart print -----");
    pSmart (&smartRoot, &root);
  #endif

  FILE *output = fopen ("./bootstrapping/output.ll", "w+");
  llGenerate (output, &smartRoot, &root);  

  for( int i = 0; i < root.length; i++)
    {
      if( BRANCHGET(root, i).type == Magic )
        /*->*/ free (BRANCHGET(root, i).saves.magic);
      if( BRANCHGET(root, i).type == LiteralType )
        /*->*/ free (BRANCHGET(root, i).saves.lit);
      else
      if( BRANCHGET(root, i).type == Definition )
        {
          // free array data memory
          free (BRANCHGET(root, i).saves.definition.array.size);
          free (BRANCHGET(root, i).saves.definition.array.type);
          
          // free definition info memory
          free (BRANCHGET(root, i).saves.definition.id);
          free (BRANCHGET(root, i).saves.definition.type);
        }
      else
      if( BRANCHGET(root, i).type == ArrayType )
        {
          free (BRANCHGET(root, i).saves.array.size);
          free (BRANCHGET(root, i).saves.array.type);
        }
    }
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