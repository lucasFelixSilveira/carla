#include <stdio.h>
#include <string.h>
#include "../utils/vector.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "debug.h"

#define TKGET(tks, i) ((Token*)tks->items)[i]
#define NGET(tks, i) ((PNode*)tks->items)[i]

char *TType[] = {
  "NODE_NUMBER",
  "NODE_ID",
  "NODE_TEXT",
  "NODE_OPERATION",
  "NODE_DEFINITION",
  "NODE_LAMBDA",
  "NODE_BEGIN",
  "NODE_END"
};

void
pTokens (Vector *tks) 
{
  for(
    int i = 0;
    i < tks->length; 
    i++
  ) {
      Token tk = TKGET(tks, i);
      printf ("Token<%d> { type: %d\tbuffer: %s\t},\n", i, tk.type, tk.buffer);
    }
}

void
pNodes (Vector *root) 
{
  for(
    int i = 0;
    i < root->length; 
    i++
  ) {
      PNode brench = NGET(root, i);
      printf ("|- %s[%d] : %d\n", TType[brench.type], brench.type, i);
      
      switch(brench.type)
        {
          case NODE_DEFINITION:
            {
              printf ("|\t|-hopeful: %s\n|\t|-type: %s\n|\t|-id: %s\n", 
                (brench.data.definition.hopeful) ? "true" : "false",
                brench.data.definition.type,
                brench.data.definition.id
              );
            } break;
          
          default: break;
        }
      
      printf ("|\n");
    }
}

void 
pHeader (int *count, char *header) 
{
  if( *count > 0 ) printf ("\n\n");
  printf ("\n%s - [%d]\n\n", header, *count);
  count += 1;
}