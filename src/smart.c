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
              else if( branch.saves.definition.hopeful == 0 ) 
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
          case Normal:
            {
              switch( branch.saves.token.type )
                {
                  case Integer:
                  case Identifier:
                    {
                      Vector expr = vector_init (sizeof (PNode));
                      EXPR_T currentID = EXPR_UNDEFINED_T;
                      EXPR_R semanticID = EXPR_UNDEFINED_R;
                      
                      if( branch.saves.token.type == Identifier )
                        /*->*/ currentID = EXPR_IDENTIFIER_T;
                      else
                      if( branch.saves.token.type == Integer )
                        /*->*/ currentID = EXPR_INT_T;
                      
                      while(1) 
                        {
                          i++;
                          PNode current = BRANCHGET(pRoot, i);

                          vector_push (&expr, (void*)((PNode*)&current));

                          if( current.type == Normal )
                            {
                              if( 
                                current.type == End  
                                || current.type == Magic 
                                || current.saves.token.type == Semi 
                              ) {
                                  if( current.type != Magic ) 
                                    /*->*/ i++;
                                  break;
                                }
                              
                              if( currentID == EXPR_IDENTIFIER_T && semanticID != EXPR_BOOL_R )
                                switch (current.saves.token.type)
                                  {
                                    case ArithmeticOperator:
                                      {
                                        currentID = EXPR_ARITHMETIC;
                                      } break;
                                    default: break;
                                  }
                              else 
                              if( currentID == EXPR_INT_T && semanticID != EXPR_BOOL_R )
                                switch (current.saves.token.type)
                                  {
                                    case ArithmeticOperator:
                                      {
                                        currentID = EXPR_ARITHMETIC;
                                      } break;
                                    case COMP_CONSTANT:
                                    case COMP_CONSTANT+1:
                                    case COMP_CONSTANT+2:
                                    case COMP_CONSTANT+3:
                                    case COMP_CONSTANT+4:
                                    case COMP_CONSTANT+5:
                                      {
                                        currentID = EXPR_COMPARATION;
                                      } break;
                                    default: break;
                                  }
                              else 
                              if( currentID == EXPR_ARITHMETIC )
                                switch (current.saves.token.type)
                                  {
                                    case Identifier:
                                      {
                                        semanticID = EXPR_INT_R;
                                        currentID = EXPR_IDENTIFIER_T;
                                      } break;
                                    case Integer:
                                      {
                                        semanticID = EXPR_INT_R;
                                        currentID = EXPR_INT_T;
                                      } break;
                                    default: break;
                                  }
                              else 
                              if( currentID == EXPR_COMPARATION )
                                switch (current.saves.token.type)
                                  {
                                    case Identifier:
                                      {
                                        semanticID = EXPR_BOOL_R;
                                        currentID = EXPR_IDENTIFIER_T;
                                      } break;
                                    case Integer:
                                      {
                                        semanticID = EXPR_BOOL_R;
                                        currentID = EXPR_INT_T;
                                      } break;
                                    default: break;
                                  }
                            }
                        }

                      vector_push (sRoot, (void*)(&(SNode) {
                        .type = SExpression, 
                        .what = (Declaration) {
                          .expr = (Expr) {
                            .semantic = semanticID,
                            .vec = &expr
                          }
                        }
                      }));
                    } break;
                  default: break;
                }
            }
          default: break;
        }
    }
}