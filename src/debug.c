#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "parser.h"
#include "debug.h"

#define TYPES_LENGTH 8
const char *__Types[TYPES_LENGTH] = {
  "Definition",
  "Expression",
  "ArrayType",
  "Lambda",
  "Normal",
  "Begin",
  "Magic",
  "End"
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
      printf ("Branch<%d> {\n\ttype: %s[%d]\n", i, __Types[branch.type], branch.type);
      
      switch(branch.type) {
        case Definition: {
          printf ("\t%s: %s\n", "hopeful", branch.saves.definition.hopeful ? "true" : "false");
          printf ("\t%s: %s\n", "expect", branch.saves.definition.type);
          printf ("\t%s: %s\n", "identifier", branch.saves.definition.id);
          printf ("\t%s: %s\n", "type", branch.saves.definition.key_type ? "true" : "false");
          printf ("\t%s: %s\n", "argument", branch.saves.definition.arg ? "true" : "false");

          if( strcmp (branch.saves.definition.type, "ptr") == 0 ) 
            {
              printf ("\t%s: {\n", "array");
              printf ("\t\t%s: %s\n", "length", branch.saves.definition.array.size);
              printf ("\t\t%s: %s\n", "type", branch.saves.definition.array.type);
              printf ("\t}\n");
            }
          break;
        }
        case ArrayType: {
          printf ("\t%s: {\n", "array");
          printf ("\t\t%s: %s\n", "length", branch.saves.definition.array.size);
          printf ("\t\t%s: %s\n", "type", branch.saves.definition.array.type);
          printf ("\t}\n");
          break;
        }
        case Normal: {
          printf ("\t%s: %s\n", "buffer", branch.saves.token.buffer);
          break;
        } 
        case Magic: {
          printf ("\t%s: %s\n", "keyword", branch.saves.magic);
          break;
        } 
        default: break;
      }
      
      printf ("},\n");
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
      printf ("Token<%d> {\n\ttype: %d\n\tbuffer: %s\n},\n", i, tk.type, tk.buffer);
    }
}

void 
pHeader (int *count, char *header) 
{
  if( *count > 0 ) printf ("\n\n");
  printf ("\n%s - [%d]\n\n", header, *count);
  count += 1;
}