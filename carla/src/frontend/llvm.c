#include <stdlib.h>
#include <string.h>
#include "../utils/vector.h"
#include "../utils/strings.h"
#include "parser.h"

#define GET(vector, index) ((PNode*)vector->items)[index]
#define STATIC_TYPE_LENGTH 3

unsigned int var = 0;
int tab = 0;

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
    { "asciz", "ptr" },
    { "ascii", "i8" },
    { "byte",  "i8" }
  };
  
  for( int i = 0; i < STATIC_TYPE_LENGTH; i++ )
    {
      if( strcmp (matrix[i][0], type) != 0 )
        continue;
      
      memcpy (__type, matrix[i][1], strlen (matrix[i][1]));
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
  printf ("puts %s\n", type);
  free (type);
  free (tabs);
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
llGenerate(FILE *output, Vector *pTree)
{
  int i = 0;
  for(; i < pTree->length; i++ )
    {
      PNode branch = GET(pTree, i);

      switch(branch.type)
        {
          /* type id (; | =) */
          case NODE_DEFINITION: 
            {
              if(! branch.data.definition.hopeful )
                {
                  llvm_alloca (output, branch);
                  continue;
                }

                if( GET(pTree, i+1).type == NODE_LAMBDA )
                  {
                    i += 2;
                    char *type = llvm_type (branch.data.definition.type);
                    char *tabs = (char*)malloc (1024);
                    genT (&tabs);
                    fprintf (output, "%sdefine %s @%s(",
                            tabs,
                            type,
                            branch.data.definition.id 
                    );
                    free (tabs);

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
                      llvm_alloca (output, arg);

                    j = 0;
                    for(; (arg = GET(pTree, j + i)).type != NODE_BEGIN; j++ )
                      llvm_store (output, arg, var - (q * 2) - 1 + j, var - q + j);

                    free (type);
                    i += q;

                    continue;
                  }
            } break;
          case NODE_END:
            {
              tab--;
              char *tabs = (char*)malloc (1024);
              genT (&tabs);
              fprintf (output, "%s}\n", tabs);
              free (tabs);
            } break;
          default: break;
        }
    }

  goto __ignore_error;
  __llvm_gen_error__: {
    printf ("Fail to generate the LLVM code\n");
  }
  __ignore_error: {}
}
