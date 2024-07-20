#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "parser.h"

void 
llGenerate (FILE *output, Vector *pTree) 
{
  for(
    int i = 0;
    i < pTree->length; 
    i++
  ) {
      PNode branch = ((PNode*)pTree->items)[i];
      if( 
        branch.type == Definition && 
        branch.saves.definition.hopeful  
      ) {
          PNode next = ((PNode*)pTree->items)[i + 1];
          if( next.type == Lambda ) {

            char *type = branch.saves.definition.type;
            char *id = branch.saves.definition.id;
            
            fprintf(output, "\ndefine %s @%s", 
                    branch.saves.definition.type, 
                    branch.saves.definition.id
            );
          }
        }
    }
}
