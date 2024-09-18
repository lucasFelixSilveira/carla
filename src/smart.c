#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smart.h"
#include "vector.h"
#include "parser.h"

#define BRANCHGET(root, i) ((PNode*) root->items)[i]

void 
sGenerate (Vector *sRoot, Vector *pRoot) 
{
  for(
    int i = 0;
    i < pRoot->length;
    i++
  ) {
      PNode branch = BRANCHGET(pRoot, i);
      
      switch( branch.type )
        {
          case Definition: 
            {
              PNode next = BRANCHGET(pRoot, i + 1);

              if( next.type == Lambda )
                {
                  i += 2;

                  int args_length = 0;
                  while( BRANCHGET(pRoot, i + args_length).type == Definition )
                    { args_length++; }
                  
                  int *args = (int*)malloc (sizeof (int) * args_length);
                  for(
                    int j = 0;
                    j < args_length;
                    j++
                  ) { args[j] = i++; }

                  vector_push (sRoot, (void*)(&(SNode) {
                    .type = DLambda,
                    .what = (Declaration) {
                      .lambda = (SLambda) {
                        .args = args,
                        .lArgs = args_length,
                        .definition = branch.saves.definition
                      }
                    }
                  }));

                  continue;
                }
            } break;
          default: break;
        }
    }
}