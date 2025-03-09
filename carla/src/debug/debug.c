#include <stdio.h>
#include <string.h>
#include "../utils/vector.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "debug.h"

#define TKGET(tks, i) ((Token*)tks->items)[i]
#define NGET(tks, i) ((PNode*)tks->items)[i]

char *TType[] = {
  "NODE_NUMBER",
  "NODE_ID",
  "NODE_TEXT",
  "NODE_OPERATION",
  "NODE_INTERNAL",
  "NODE_INTERNAL_SUPER",
  "NODE_DEFINITION",
  "NODE_DEF_LIBC",
  "NODE_SINGLE",
  "NODE_ACCESS",
  "NODE_LAMBDA",
  "NODE_BEGIN",
  "NODE_BEGIN_IMPLMENT",
  "NODE_TYPE",
  "NODE_FOR",
  "NODE_IF",
  "NODE_ELSE",
  "NODE_OUR",
  "NODE_STRUCT",
  "NODE_CLOSE",
  "NODE_RET",
  "NODE_END",
  "NODE_END_IMPLEMENT",
  "NODE_INTERNAL_STRUCT",
  "NODE_FIELD_ACCESS",
  "NODE_ASSIGNMENT",
  "NODE_METHOD",
  "NODE_EEXP"
};

void
pTokens (Vector *tks) 
{
  for(
    int i = 0;
    i < tks->length; 
    i++
  ) {
      Token tk = TKGET(tks, i);
      printf ("Token<%d> {\n\ttype: %d\n\tbuffer: %s\n},\n", i, tk.type, tk.buffer);
    }
}

void
pNodes (Vector *root) 
{
  for(
    int i = 0;
    i < root->length; 
    i++
  ) {
      PNode brench = NGET(root, i);
      printf ("|- %s[%d] : %d\n", TType[brench.type], brench.type, i);
      
      switch(brench.type)
        {
          case NODE_DEF_LIBC:
          case NODE_DEFINITION:
            {
              printf ("|\t|-hopeful: %s\n|\t|-type: %s\n|\t|-id: %s\n|\t|-bound: %s\n", 
                     (brench.data.definition.hopeful) ? "true" : "false",
                     brench.data.definition.type,
                     brench.data.definition.id,
                     (brench.data.definition.is_bound) ? "true" : "false"
              );
            } break;
          case NODE_ACCESS:
            {
              printf ("|\t|-vector: %s\n", 
                     brench.data.value
              );
            } break;
          case NODE_TYPE:
            {
              printf ("|\t|-type: %s\n", 
                     brench.data.value
              );
            } break;
          case NODE_LAMBDA:
            {
              printf ("|\t|-our: %s\n", 
                     (brench.data.our) ? "true" : "false"
              );
            } break;
          case NODE_FIELD_ACCESS:
            {
              printf ("|\t|-access: %s\n", 
                brench.data.value
              );
            } break;
          case NODE_ASSIGNMENT:
            {
              printf ("|\t|-assignment: %s\n", 
                brench.data.value
              );
            } break;
          case NODE_STRUCT:
            {
              printf ("|\t|-name: %s\n", 
                     brench.data.value
              );
            } break;
          case NODE_INTERNAL:
          case NODE_INTERNAL_SUPER:
            {
              printf ("|\t|-function: %s\n|\t|-library: %s\n", 
                     brench.data.internal.function,
                     brench.data.internal.lib
              );
            } break;

          case NODE_OPERATION:
            {
              printf ("|\t|-left: %s<%s>\n|\t|-operator: %s\n|\t|-right: %s<%s>\n", 
                     brench.data.operation.left.buffer,
                     brench.data.operation.left.type == Integer ? "INT" : "ID",
                     brench.data.operation.operation.buffer,
                     brench.data.operation.right.buffer,
                     brench.data.operation.right.type == Integer ? "INT" : "ID"
              );
            } break;
          
          default: break;
        }
      
      printf ("|\n");
    }
}

void 
pHeader (int *count, char *header) 
{
  if( *count > 0 ) printf ("\n\n");
  printf ("\n%s - [%d]\n\n", header, *count);
  count += 1;
}