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

  Vector root = vector_init(sizeof (PNode));
  generate (&root, &tokens);
  
  #if DEBUG 
    
    printf ("\n\n----- Tree print: -----\n");
    
    for(
      int i = 0;
      i < root.length; 
      i++
    ) {
        PNode branch = ((PNode*)root.items)[i];
        printf("Branch<%d> {\n\ttype: %d\n},\n", i, branch.type);
      }
  #endif

  fclose (main);
  return 2;
}