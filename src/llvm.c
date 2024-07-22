#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "parser.h"

typedef struct {
  DMemory def;
  int level;
  int llvm;
} Variable;

unsigned short stackpos = 0;
unsigned short varspos  = 0;
char *stacktype[1024];
Variable variables[2048];
int scope = 0;

int
llvm_sizeof (char *type)
{
  if( strcmp (type, "i8") == 0 || strcmp (type, "u8") == 0 )
    return 1;
  if( strcmp (type, "i16") == 0 || strcmp (type, "u16") == 0 )
    return 2;
  if( strcmp (type, "i32") == 0 || strcmp (type, "u32") == 0 )
    return 4;
  if( strcmp (type, "i64") == 0 || strcmp (type, "u64") == 0 )
    return 8;
  if( strcmp (type, "i128") == 0 || strcmp (type, "u128") == 0 )
    return 16;
}

void 
llGenerate (FILE *output, Vector *pTree) 
{
  int var = 1;

  for(
    int i = 0;
    i < pTree->length; 
    i++
  ) {
      PNode branch = ((PNode*)pTree->items)[i];
      if( 
        branch.type == Definition
      ) {
          if( 
            branch.saves.definition.hopeful 
            && ((PNode*)pTree->items)[i + 1].type == Lambda 
          ) {
              char *type = branch.saves.definition.type;
              char *id = branch.saves.definition.id;
              
              stacktype[stackpos++] = strdup (type);
              fprintf (output, "define %s @%s(", 
                      branch.saves.definition.type, 
                      branch.saves.definition.id
              );

              for(
                int j = i + 2;
                j < pTree->length;
                j++ 
              ) {
                  PNode branch = ((PNode*)pTree->items)[j];
                  if( branch.type == Begin )
                    /*->*/ break;
                }

              scope++;
              fprintf (output, ") {\n");
            }
          else
            {
              fprintf (output, "%c%d = alloca %s, align %d\n", '%', 
                      var, 
                      branch.saves.definition.type,
                      llvm_sizeof (branch.saves.definition.type)
              );
              
              variables[varspos++] = (Variable) {
                .level = scope, 
                .def = branch.saves.definition,
                .llvm = var++
              };

              PNode next = ((PNode*)pTree->items)[i + 1];
              if( 
                next.type == Normal 
                && next.saves.token.type == Integer 
              ) {
                  fprintf (output, "store %s %s, ptr %c%d, align %d\n", 
                          branch.saves.definition.type,
                          next.saves.token.buffer, '%',
                          (var - 1),
                          llvm_sizeof (branch.saves.definition.type)        
                  );
                }
              i++;
            }
        }
      else
      if( 
        branch.type == Magic 
        && strcmp (branch.saves.magic, "return") == 0
      ) {
          
          PNode next = ((PNode*)pTree->items)[i + 1];

          if( next.type == Normal && next.saves.token.type == Integer ) 
            {
              fprintf (output, "ret %s %s\n", 
                      stacktype[--stackpos], 
                      next.saves.token.buffer
              );
            }
          else
          if( next.type == Normal && next.saves.token.type == Identifier ) 
            {
              for(
                int i = 0;
                i < varspos;
                i++
              ) {
                  if( variables[i].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[i].def.id, next.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  int size_var = llvm_sizeof (variables[i].def.type);
                  int size_last = llvm_sizeof (stacktype[--stackpos]);

                  if( size_var == size_last ) {
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var, 
                            stacktype[stackpos], '%',
                            variables[i].llvm,
                            size_last
                    );

                    fprintf (output, "ret %s %c%d\n", 
                            stacktype[stackpos], '%', 
                            var++
                    );
                  } else if( size_var < size_last ) {
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var, 
                            variables[i].def.type, '%', 
                            variables[i].llvm, 
                            size_var
                    );

                    fprintf (output, "%c%d = sext %s %c%d to %s\n", '%',
                            var + 1, 
                            variables[i].def.type, '%',
                            var, 
                            stacktype[stackpos]
                    );

                    fprintf (output, "ret %s %c%d\n", 
                            stacktype[stackpos], '%',
                            var + 1
                    );
                    var += 2;
                  } else {
                    fprintf (output, "ret %s\n", 
                            stacktype[stackpos]
                    );
                  }
                  i++;
                }
            }

          i++;
          stacktype[stackpos] = NULL;
        }
      else
      /*
        identifier operator(=) value
      */
      if( 
        branch.type == Normal 
        && branch.saves.token.type == Identifier 
        && ((PNode*)pTree->items)[i + 1].type == Normal
        && strcmp (((PNode*)pTree->items)[i + 1].saves.token.buffer, "=") == 0
      ) {
          i++;

          expr_t expr_type = ENone;
          int x = (i + 1);
          for(
            ;1;
            x++
          ) {
              PNode value = ((PNode*)pTree->items)[x];

              if( value.type == Normal && expr_type == ENone ) 
                {
                  expr_type = ELit;
                  continue;
                }
              else 
                { break; }

            }
          
          for(
                int y = 0;
                y < varspos;
                y++
              ) {
                  if( variables[y].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[y].def.id, branch.saves.token.buffer) != 0 )
                    /*->*/ continue; 
                    
                  switch(expr_type)
                    {
                      case ELit: 
                        {
                          fprintf (output, "store %s %s, ptr %c%d, align %d\n", 
                                  variables[y].def.type,
                                  ((PNode*)pTree->items)[++i].saves.token.buffer, '%',
                                  variables[y].llvm,
                                  llvm_sizeof (variables[y].def.type)
                          );

                          break;
                        }
                      default:
                        {
                          printf ("Ocorred a problem");
                          exit (0);
                          break;
                        }
                    }
              }
        }
      else
      if( branch.type == End )
        { 
          fprintf (output, "}\n");
          scope--;

          for(
            int i = varspos;
            i > 0;
            i--
          ) {
              if( variables[i].level > scope ) 
                {
                  var--;
                  varspos--;
                }

              if( variables[i].level < scope )
                /*->*/ break;
            }
        }
    }
}
