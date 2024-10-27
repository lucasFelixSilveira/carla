#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../utils/vector.h"
#include "../utils/strings.h"
#include "../utils/symbols.h"
#include "parser.h"
#include "llvm.h"
#include "std.h"

#define GET(vector, index) ((PNode*)vector->items)[index]
#define GETNP(type, vector, index) ((type*)vector.items)[index]
#define STATIC_TYPE_LENGTH 2

unsigned int var = 0;
int tab = 0;

ExprCache cache[1024];
struct ARG {
  unsigned int llvm;
  char *arg_type;
} args[128];
int alen;
int clen;

void
genT(char **tabs)
{
  if( tab == 0 )
    (*tabs)[0] = 0x0;
  else 
    {
      int i = 0;
      for(; i < tab; i++)
        {
          (*tabs)[i] = 0x9;
          (*tabs)[i+1] = 0x0;
        }
    }
}

int
llvm_sizeof (char *type)
{
  if( type[0] == 'i' )
    {
      char *bits = substr (type, 1, strlen (type) - 1);
      int bytes = atoi (bits) / 8;
      return bytes;
    }

  if( strcmp (type, "ptr") == 0 ) 
    return 8;
  
  return 0;
}

char *
llvm_type (char *type) 
{
  const char ptrsufix = '*';
  if( type[strlen (type) - 1] == ptrsufix )
    return "ptr";

  const char arrayprefix = '[';
  if( type[0] == arrayprefix )
    return "ptr";

  char *__type = (char*)malloc (128);
  const char *prefix = "int";
  for( char i = 0; i < 3; i++ )
    {
      if( prefix[i] != type[i] )
        break;
      if( i == 2 )
        {
          char *bits = substr (type, 3, strlen (type) - 2);
          char *result = (char*)malloc (strlen (type));
          sprintf (result, "i%s", bits);
          free (bits);
          free (__type);
          return result;
        }
    }

  if( strcmp (type, "void") == 0 )
    {
      memcpy (__type, type, strlen (type));
      return __type;
    }
  
  char *matrix[STATIC_TYPE_LENGTH][16] = {
    { "ascii", "i8" },
    { "byte",  "i8" }
  };
  
  for( int i = 0; i < STATIC_TYPE_LENGTH; i++ )
    {
      if( strcmp (matrix[i][0], type) != 0 )
        continue;
      
      memcpy (__type, matrix[i][1], strlen (matrix[i][1]));
      __type[strlen (matrix[i][1])] = 0;
      return __type;
    }

  return strdup ("void");
}

void
llvm_alloca(FILE *output, PNode node)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n", 
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_alloca_t(FILE *output, char *__type)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (__type);
  fprintf (output, "%s%c%d = alloca %s, align %d\n", 
          tabs, '%',
          var++,
          type,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

char *
backPtr(char *type) 
{
  if( isType (type) )
    return type;

  if( type[0] == '[' ) 
    {
      uint16_t len = 0;
      while(type[len++] != ']');
      return substr (type, len, (strlen (type) - len));
    }

  if( type[strlen (type) - 1] == '*' ) 
    {
      return substr (type, 0, (strlen (type) - 1));
    }

  return "void";
}

void
llvm_store(FILE *output, PNode node, int __src_var__, int __dst_var__)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  char *type = llvm_type (node.data.definition.type);
  fprintf (output, "%sstore %s %c%d, ptr %c%d, align %d\n", 
          tabs, 
          type, '%',
          __src_var__, '%',
          __dst_var__,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_store_l(FILE *output, int bits, int number, uint32_t dist)
{
  char *tabs = (char*)malloc (1024);
  genT (&tabs);
  fprintf (output, "%sstore i%d %d, ptr %c%d, align %d\n", 
          tabs, 
          bits,
          number, '%',
          dist,
          (bits / 8)
  );
  free (tabs);
}

Variable
llvm_get(Vector *vec, char *id)
{
  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return var;
    }
  
  printf ("%s NOT FOUND!", id);
  exit (0);
}

uint32_t
llvm_get_i(Vector *vec, char *id)
{
  for(
    int i = 0; 
    i < vec->length; 
    i++
  ) {
      Variable var = ((Variable*)vec->items)[i];
      if( strcmp (var.id, id) == 0 )
        return i;
    }
  
  printf ("%s NOT FOUND!", id);
  exit (0);
}

void 
llvm_load(FILE *output, Variable toLoad)
{
  char *tabs = (char*)malloc (1024);
  char *type = llvm_type (toLoad.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = load %s, ptr %c%d, align %d\n", 
          tabs, '%',
          var++,
          type, '%',
          toLoad.llvm,
          llvm_sizeof (type)
  );
  free (type);
  free (tabs);
}

void
llvm_getelementptr(FILE *output, ExprCache vec) 
{
  char *tabs = (char*)malloc (1024);
  char *type = llvm_type (vec.info.access.type);
  genT (&tabs);
  fprintf (output, "%s%c%d = getelementptr inbounds %s, %s %c%d, i64 %c%d\n", 
          tabs, '%',
          var,
          type, 
          llvm_type (vec.info.access.type), '%',
          vec.info.access.load_vec, '%',
          (var-1)
  );
  var++;
  free (type);
  free (tabs);
}

void
llGenerate(FILE *output, Vector *pTree)
{

  Vector     vars   = vector_init (sizeof (Variable));
  Vector     scopes = vector_init (sizeof (ScopeType));
  uint32_t **hoping = (uint32_t**)malloc ((512 * 3) * sizeof (int));
  uint16_t   hope   = 0;

  int i = 0;
  for(; i < pTree->length; i++ )
    {
      PNode branch = GET(pTree, i);

      goto jmp_expr;
      wait_expr: 
        {
          uint16_t old_h = hope;
          while(1) 
            {
              PNode next = GET(pTree, i + 1);
              
              switch (next.type)
                {
                  case NODE_ACCESS:
                    {
                      i++;
                      Variable access = llvm_get (&vars, next.data.value);

                      llvm_load (output, access);
                      cache[clen++] = (ExprCache) {
                        .type = ACCESS_EXPR,
                        .info = {
                          .access = {
                            .load_vec = (var-1),
                            .type     = access.type,
                            .id       = access.id
                          }
                        }
                      };

                    } break;

                  case NODE_SINGLE:
                    {
                      i++;
                      switch(next.data.single.type)
                        {
                          case NODE_NUMBER: 
                            {
                              llvm_alloca_t (output, "int64");
                              llvm_store_l (output, 64, (int)next.data.single.data.number, (var - 1));
                            } break;
                          case NODE_ID:
                            {
                              Variable id = llvm_get (&vars, next.data.value);
                              llvm_load (output, id);
                            }
                          default: break;
                        }

                      ExprCache resolve = cache[--clen];
                      switch (resolve.type)
                        {
                          case ACCESS_EXPR:
                            {
                              llvm_load (output, (Variable) { .llvm = (var-1), .type = "int64"});
                              llvm_getelementptr (output, resolve);
                              llvm_load (output, (Variable) { .llvm = (var-1), .type = backPtr(resolve.info.access.type) });
                            } break;

                          case FUNCTION_CALL:
                            {
                              // args[alen][0] = resolve.info.fn.llvm_id;
                              args[alen++] = (struct ARG) {
                                .llvm = (var - 1),
                                .arg_type = (next.data.single.type == NODE_ID) ? "[]byte" : "int64"
                              };
                              clen++;
                            } break;
                          
                          default:
                            break;
                        }

                    } break;

                  case NODE_INTERNAL:
                    {
                      i++;
                      cache[clen++] = (ExprCache) {
                        .type = FUNCTION_CALL,
                        .info = {
                          .fn_call = std_fn (branch)
                        }
                      };
                    } break;

                  case NODE_CLOSE: 
                    {
                      i++;
                      ExprCache resolve = cache[--clen];
                      if( resolve.type != FUNCTION_CALL )
                        goto __llvm_gen_error__;
                      
                      char *tabs = (char*)malloc (1024);
                      genT (&tabs);
                      if( strcmp (resolve.info.fn_call.type, "void") != 0 )
                        {
                          fprintf (output, "%s%c%d = ", 
                                  tabs, '%',
                                  var++
                          );
                        } 
                      else 
                        fprintf (output, "%s", tabs);
                      free (tabs);

                      fprintf (output, "call %s @%s.%s(",
                              resolve.info.fn_call.type, 
                              resolve.info.fn_call.lib,
                              resolve.info.fn_call.id
                      );

                      int j = 0;
                      for(; j < alen; j++ )
                        {
                          fprintf (output, "%s%s %c%d", 
                            (j == 0) ? "" : ", ",
                            llvm_type (args[j].arg_type), '%',
                            args[j].llvm
                          );
                        }

                      fprintf (output, ")\n");
                      break;
                    }
                  
                  default: 
                    goto finish;
                }

              continue;
              finish: { break; }
            }
          
          if( hope > 0 )
            {
              uint32_t *last = hoping[--hope];
              
              llvm_store (output, GET(pTree, last[1]), (var - 1), last[0]);
            }
          continue;
        }
      jmp_expr: {}

      switch(branch.type)
        {
          /* type id (; | =) */
          case NODE_DEFINITION: 
            {

              if( GET(pTree, i+1).type == NODE_LAMBDA && tab == 0 )
                {
                  i += 2;
                  char *type = llvm_type (branch.data.definition.type);
                  char *tabs = (char*)malloc (1024);
                  genT (&tabs);
                  fprintf (output, "\n%sdefine %s @%s(",
                          tabs,
                          type,
                          branch.data.definition.id 
                  );
                  free (tabs);

                  vector_push (&vars, ((void*)&(Variable) {
                    .v_type = Function, 
                    .type   = branch.data.definition.type,
                    .llvm   = 0,
                    .tab    = tab,
                    .id     = branch.data.definition.id
                  }));

                  int j = 0;
                  PNode arg;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      switch(arg.type)
                        {
                          case NODE_DEFINITION:
                            {
                              char *prefix = (char*)malloc (3); 
                              if( j > 0 ) 
                                sprintf (prefix, ", ");
                              else memset (prefix, 0, 3);
                              char *type = llvm_type (arg.data.definition.type);
                              fprintf (output, "%s%s %c%d",
                                      prefix,
                                      type, '%',
                                      var++
                              );
                              free (prefix);
                            } break;
                          
                          default: {
                            goto __llvm_gen_error__;
                          } break;
                        }
                    }
                  int q = j;

                  fprintf (output, ") {\n");
                  var++;
                  tab++;

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    {
                      vector_push (&vars, ((void*)&(Variable) {
                        .v_type = Normal, 
                        .type   = arg.data.definition.type,
                        .llvm   = var,
                        .tab    = tab,
                        .id     = arg.data.definition.id
                      }));
                      llvm_alloca (output, arg);
                    }

                  j = 0;
                  for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                    llvm_store (output, arg, var - (q * 2) - 1 + j, var - q + j);

                  free (type);
                  i += q;

                  ScopeType stype = Lambda;
                  vector_push (&scopes, ((void*)&stype));

                  continue;
                }

              vector_push (&vars, ((void*)&(Variable) {
                .v_type = Normal, 
                .type   = branch.data.definition.type,
                .llvm   = var,
                .tab    = tab,
                .id     = branch.data.definition.id
              }));

              llvm_alloca (output, branch);
              if(! branch.data.definition.hopeful )
                continue;

              uint32_t *append = (uint32_t*) malloc (8);
              append[0] = (var - 1);
              append[1] = i; 
              hoping[hope++] = append;
              
              goto wait_expr;
              
            } break;

          case NODE_END:
            {
              tab--;
              char *tabs = (char*)malloc (1024);
              genT (&tabs);
              if( tab == 0 )
                var = 0;

              int index = scopes.length - 1;
              switch(GETNP(ScopeType, scopes, index))
                {
                  case Lambda:
                    {
                      fprintf (output, "}\n");
                    } break;
                  default: break;
                }              
              
              vector_remove (&scopes, index);

              
              int last;
              Variable *__var;
              while( last < 0 && (__var = &GETNP(Variable, vars, (last = (vars.length - 1))))->tab > tab )
                {
                  free (__var->type);
                  free (__var->id);
                  vector_remove (&scopes, last);
                }

              free (tabs);
            } break;
          case NODE_INTERNAL:
            { 
              i--;
              goto wait_expr;
            }
          default: break;
        }
    }

  goto __ignore_error;
  __llvm_gen_error__: {
    printf ("Fail to generate the LLVM code\n");
  }
  __ignore_error: {}

  int j = 0;
  for(; j < vars.length; j++ )
    {
      free (GETNP(Variable, vars, j).type);
      free (GETNP(Variable, vars, j).id);
    }
  vector_free (&vars);
  vector_free (&scopes);
  
  j = 0;
  for(; j < hope; j++ )
    free (hoping[j]);
  free (hoping);
}