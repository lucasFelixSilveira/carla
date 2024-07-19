#include <stdio.h>
#include "vector.h"
#include "parser.h"
#include "debug.h"

#define TYPES_LENGTH 2
const char *types[TYPES_LENGTH] = {
  "Definition",
  "Magic"
};

void
pRoot (Vector *root) 
{
  for(
    int i = 0;
    i < root->length; 
    i++
  ) {
      PNode branch = ((PNode*)root->items)[i];
      printf("Branch<%d> {\n\ttype: %s[%d]\n", i, types[branch.type], branch.type);
      
      switch(branch.type) {
        case Definition: {
          printf("\t%s: %s\n", "hopeful", branch.saves.definition.hopeful ? "true" : "false");
          printf("\t%s: %s\n", "expect", branch.saves.definition.type);
          printf("\t%s: %s\n", "identifier", branch.saves.definition.id);
          break;
        }
        case Magic: {
          printf("\t%s: %s\n", "keyword", branch.saves.magic);
          break;
        } 
      }
      
      printf("},\n");
    }
}

void
pTokens (Vector *tks) 
{
  for(
    int i = 0;
    i < tks->length; 
    i++
  ) {
      Token tk = ((Token*)tks->items)[i];
      printf("Token<%d> {\n\ttype: %d\n\tbuffer: %s\n},\n", i, tk.type, tk.buffer);
    }
}

void 
pHeader (int *count, char *header) 
{
  if( *count > 0 ) printf("\n\n");
  printf("%s - [%d]\n\n", header, *count);
  count += 1;
}