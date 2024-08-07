#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "parser.h"
#include "symbols.h"

typedef struct {
  DMemory def;
  int level;
  int llvm;
} Variable;

typedef enum {
  TArray
} TType;

typedef struct {
  char *id;
  char *respective;
  char *natural;
  char *util;
  TType strict;
} Type;

typedef struct {
  char *type;
  char *length;
} Vectorize;


int stackpos = 0;
int varspos  = 0;
int types_len = 0;
char *stacktype[1024];
Variable variables[2048];
Type types[4096];
int scope = 1;

char *opcodes[4] = {
  "sub", "add", "mul", "div"
};

int
llvm_sizeof (char *type)
{
  if( strcmp (type, "i8") == 0 || strcmp (type, "u8") == 0 )
    return 1;
  if( strcmp (type, "i16") == 0 || strcmp (type, "u16") == 0 )
    return 2;
  if( strcmp (type, "i32") == 0 || strcmp (type, "u32") == 0 )
    return 4;
  if( strcmp (type, "i64") == 0 || strcmp (type, "u64") == 0 || strcmp (type, "ptr") == 0 )
    return 8;
  if( strcmp (type, "i128") == 0 || strcmp (type, "u128") == 0 )
    return 16;
  return 0;
}

void 
llGenerate (FILE *output, char *directory, Vector *pTree) 
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
          if( branch.saves.definition.key_type )
            {
              PNode t = ((PNode*)pTree->items)[i+1];
              if( t.type == ArrayType )
                {
                  char *op1 = (char*)malloc (64);
                  char *op2 = (char*)malloc (64);

                  sprintf (op1, "%s*", t.saves.definition.array.type);
                  sprintf (op2, "[%s x %s]", t.saves.definition.array.size, t.saves.definition.array.type);

                  fprintf(output, "%c%s = type %s\n", '%',
                          branch.saves.definition.id,
                          strcmp (t.saves.definition.array.size, "undefined") == 0 ? op1 : op2
                  );

                  if( strcmp (t.saves.definition.array.size, "undefined") == 0 )
                    { free (op2); }
                  else 
                    { free (op1); }
                }
              else {
                fprintf (output, "\n\ndo nothing\n\n");
              }

              continue;
            }

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

              char *prefix = "";
              int j;
              int old = var;
              i += 2;
              for(
                j = i;
                j < pTree->length;
                j++ 
              ) {
                  PNode branch = ((PNode*)pTree->items)[j];
                  if( branch.type == Begin )
                    {
                      fprintf (output, ") {\n");
                      int x = i;
                      if( x < j ) { var++; } 
                      for(
                        ;x < j;
                        x++
                      ) {
                          PNode branch = ((PNode*)pTree->items)[x];

                          fprintf (output, "%c%d = alloca %s, align %d\n", '%', 
                                  var, 
                                  branch.saves.definition.type,
                                  llvm_sizeof (branch.saves.definition.type)
                          );

                          fprintf (output, "store %s %c%d, ptr %c%d, align %d\n",
                                  branch.saves.definition.type, '%',
                                  old + (x - i), '%',
                                  var,
                                  llvm_sizeof (branch.saves.definition.type)
                          );

                          variables[varspos++] = (Variable) {
                            .level = scope, 
                            .def = branch.saves.definition,
                            .llvm = var++
                          };
                        }
                      break;
                    }

                  if( branch.type == Definition && branch.saves.definition.arg ) 
                    {
                      if( strlen (prefix) == 1 )
                        {
                          prefix = (char*)malloc (3);
                          sprintf(prefix, ", ");
                        }

                      if( isType (branch.saves.definition.type) )
                        {

                          fprintf (output, "%s%s noundef %c%d",
                                  prefix,
                                  branch.saves.definition.type, '%', 
                                  var++
                          );

                        }     
                      else 
                      if( strcmp (branch.saves.definition.type, "ptr") == 0 )
                        {

                          if( strcmp (branch.saves.definition.array.size, "undefined") == 0 )
                            {
                              fprintf (output, "%s%c%s* noundef %c%d",
                                      prefix, '%',
                                      branch.saves.definition.array.type, '%', 
                                      var++
                              );
                            }
                          else
                            {
                              fprintf (output, "%s[%s x %c%s] noundef %c%d",
                                      prefix, 
                                      branch.saves.array.size, '%',
                                      branch.saves.definition.array.type, '%', 
                                      var++
                              );
                            }

                        }        
                      else
                        {

                          fprintf (output, "%s%c%s noundef %c%d",
                                  prefix, '%',
                                  type, '%', 
                                  var++
                          );

                        }

                      if( strlen (prefix) == 0 )
                        /*->*/ prefix = ".";

                    }
                }

              i = j;
              scope++;
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
              else
              if( 
                next.type == Normal 
                && next.saves.token.type == Identifier 
              ) {
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
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var,
                            variables[i].def.type, '%',
                            variables[i].llvm,
                            size_var    
                    );

                    int cast = 0;
                    if( size_var > llvm_sizeof (branch.saves.definition.type) ) 
                      {
                        cast = 1;
                        fprintf (output, "%c%d = trunc %s %c%d to %s\n", '%',
                                var + 1,
                                variables[i].def.type, '%',
                                var,
                                branch.saves.definition.type    
                        );
                      }
                    else 
                    if( size_var < llvm_sizeof (branch.saves.definition.type) ) 
                      {
                        cast = 1;
                        fprintf (output, "%c%d = sext %s %c%d to %s\n", '%',
                                var + 1,
                                variables[i].def.type, '%',
                                var,
                                branch.saves.definition.type    
                        );
                      }

                      fprintf (output, "store %s %c%d, ptr %c%d, align %d\n", 
                              branch.saves.definition.type, '%',
                              var + cast, '%',
                              var - 1,
                              llvm_sizeof (branch.saves.definition.type)        
                      );

                    var = var + 1 + cast;
                  }
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
                  } else {
                    fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                            var, 
                            variables[i].def.type, '%', 
                            variables[i].llvm, 
                            size_var
                    );

                    fprintf (output, "%c%d = %s %s %c%d to %s\n", '%',
                            var + 1, 
                            ( size_var > size_last ? "trunc" : "sext"),
                            variables[i].def.type, '%',
                            var, 
                            stacktype[stackpos]
                    );

                    fprintf (output, "ret %s %c%d\n", 
                            stacktype[stackpos], '%',
                            var + 1
                    );
                    var += 2;
                  }
                  i++;
                }
            }

          i++;
          stacktype[stackpos] = NULL;
        }
      /*
        identifier operator(=) number
      */
      else
      if( 
        branch.type == Normal 
        && branch.saves.token.type == Identifier 
        && ((PNode*)pTree->items)[i + 1].type == Normal
        && (
          strcmp (((PNode*)pTree->items)[i + 1].saves.token.buffer, "=") == 0
          || (
            ((PNode*)pTree->items)[i + 1].saves.token.type >= OP_CONSTANT
            && ((PNode*)pTree->items)[i + 2].saves.token.type == Integer
          )
        )
      ) {
          i++;

          if( strcmp (((PNode*)pTree->items)[i].saves.token.buffer, "=") == 0 )
            {
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
            {
              PNode operator = ((PNode*)pTree->items)[i];
              char *operation = opcodes[operator.type - Sub];

              for(
                int y = 0;
                y < varspos;
                y++
              ) {
                  if( variables[y].level > scope )
                    /*->*/ break;

                  if( strcmp (variables[y].def.id, branch.saves.token.buffer) != 0 )
                    /*->*/ continue; 

                  fprintf (output, "%c%d = load %s, ptr %c%d, align %d\n", '%',
                          var++, 
                          variables[y].def.type, '%',
                          variables[y].llvm,
                          llvm_sizeof (variables[y].def.type)
                  );

                  char changed = 0;
                  if( llvm_sizeof (variables[y].def.type) < 4 ) 
                    {
                      changed++;


                      fprintf (output, "%c%d = sext %s %c%d to i32\n", '%',
                              var,
                              variables[y].def.type, '%',
                              (var - 1)
                      );
                    }
                  if( llvm_sizeof (variables[y].def.type) > 4 ) 
                    {
                      changed++;


                      fprintf (output, "%c%d = trunc %s %c%d to i32\n", '%',
                              var,
                              variables[y].def.type, '%',
                              (var - 1)
                      );
                    }

                  fprintf (output, "%c%d = %s nsw i32 %c%d, %s\n", '%', 
                          (var + changed),
                          opcodes[operator.saves.token.type - Sub], '%',
                          // variables[y].def.type, '%',
                          (var + changed) - 1,
                          ((PNode*)pTree->items)[++i].saves.token.buffer
                  ); 

                  if( changed ) 
                    {
                      fprintf (output, "%c%d = %s i32 %c%d to %s\n", '%',
                              (var + 2), 
                              (llvm_sizeof (variables[y].def.type) > 4 ? "sext" : "trunc"), '%', 
                              (var + changed),
                              variables[y].def.type
                      );
                    }

                  fprintf (output, "store %s %c%d, ptr %c%d, align %d\n", 
                          variables[y].def.type, '%',
                          (var + changed + 1 + (changed ? 0 : -1)), '%',
                          variables[y].llvm,
                          llvm_sizeof (variables[y].def.type)
                  );

                  var += 2 + changed;
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
