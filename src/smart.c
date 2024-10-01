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
              else 
                {
                  vector_push (sRoot, (void*)(&(SNode) {
                    .type = SDefinition,
                    .what = (Declaration) {
                      .definition = branch.saves.definition
                    }
                  }));
                }
            } break;
          case End: 
            {
              vector_push (sRoot, (void*)(&(SNode) {
                .type = CloseScope,
                .what = (Declaration) {
                  .nothing = NULL
                }
              }));
            } break;
          case Magic:
            {
              vector_push (sRoot, (void*)(&(SNode) {
                .type = SMagic,
                .what = (Declaration) {
                  .magic = branch.saves.magic
                }
              }));
            } break;

          case ForceCast:
          case Normal:
            {
              if( branch.saves.token.buffer[0] == ';' )
                break;
              if( strlen (branch.saves.token.buffer) == 0 )
                break;

              Expr expr;
              switch(branch.type)
                {
                  case ForceCast:
                    {
                      expr = (Expr) {
                        .type = CastExpr,
                        .value = (expr_v) {
                          .cast = branch
                        }
                      };
                    } break;
                    
                  case Normal:
                    {

                      PNode next = BRANCHGET(pRoot, i + 1);
                      if( next.type == Normal && next.saves.token.type == Semi )
                        {
                          expr = (Expr) {
                            .type = SingleExpr,
                            .value = (expr_v) {
                              .single = branch.saves.token.buffer
                            }
                          };

                          break;
                        }
                    
                    } break;


                  default: break;
                }              

              vector_push (sRoot, (void*)(&(SNode) {
                .type = SExprNode, 
                .what = (Declaration) {
                  .expr = expr
                }
              }));
            } break;
          default: break;
        }
    }
}