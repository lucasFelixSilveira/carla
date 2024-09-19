#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "parser.h"
#include "smart.h"
#include "debug.h"

#define SMARTBRANCHGET(root, i) ((SNode*)root->items)[i]
#define BRANCHGET(root, i) ((PNode*)root->items)[i]
#define TKGET(tks, i) ((Token*)tks->items)[i]

#define TYPES_LENGTH 9
const char *__Types[TYPES_LENGTH] = {
  "LiteralType",
  "Definition",
  "Expression",
  "ArrayType",
  "Lambda",
  "Normal",
  "Begin",
  "Magic",
  "End"
};

#define SMART_TYPES_LENGTH 3
const char *__SmartTypes[SMART_TYPES_LENGTH] = {
  "Lambda",
  "Definition",
  "CloseScope"
};

void
pRoot (Vector *root) 
{
  for(
    int i = 0;
    i < root->length; 
    i++
  ) {
      PNode branch = BRANCHGET(root, i);
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
        } break;
        case ArrayType: {
          printf ("\t%s: {\n", "array");
          printf ("\t\t%s: %s\n", "length", branch.saves.definition.array.size);
          printf ("\t\t%s: %s\n", "type", branch.saves.definition.array.type);
          printf ("\t}\n");
        } break;
        case LiteralType: {
          printf ("\t%s: %s\n", "type", branch.saves.lit);
        } break;
        case Normal: {
          printf ("\t%s: %s\n", "buffer", branch.saves.token.buffer);
        } break;
        case Magic: {
          printf ("\t%s: %s\n", "keyword", branch.saves.magic);
        } break;
        default: break;
      }
      
      printf ("},\n");
    }
}

void
pSmart (Vector *root, Vector *subRoot) 
{
  for(
    int i = 0;
    i < root->length; 
    i++
  ) {
      SNode branch = SMARTBRANCHGET(root, i);
      printf ("SmartBranch<%d> {\n\ttype: %s[%d]\n", i, __SmartTypes[branch.type], branch.type);
      
      switch(branch.type) {
        case SDefinition: {
          printf ("\t%s: %s\n", "hopeful", branch.what.definition.hopeful ? "true" : "false");
          printf ("\t%s: %s\n", "expect", branch.what.definition.type);
          printf ("\t%s: %s\n", "identifier", branch.what.definition.id);
          printf ("\t%s: %s\n", "type", branch.what.definition.key_type ? "true" : "false");
          printf ("\t%s: %s\n", "argument", branch.what.definition.arg ? "true" : "false");

          if( strcmp (branch.what.definition.type, "ptr") == 0 ) 
            {
              printf ("\t%s: {\n", "array");
              printf ("\t\t%s: %s\n", "length", branch.what.definition.array.size);
              printf ("\t\t%s: %s\n", "type", branch.what.definition.array.type);
              printf ("\t}\n");
            }
        } break;
        case DLambda: {
          printf ("\t%s: %s\n", "with args", branch.what.lambda.lArgs > 0 ? "true" : "false");
          printf ("\t%s: %d\n", "args length", branch.what.lambda.lArgs);
          if( branch.what.lambda.lArgs > 0 ) 
            {
              for( 
                int i = 0;
                i < branch.what.lambda.lArgs;
                i++
              ) {
                  int *args = branch.what.lambda.args;
                  PNode arg = BRANCHGET(subRoot, args[i]);
                  printf ("\t%s: [\n", "args");
                  printf ("\t\t{\n");
                  printf ("\t\t\t%s: %s\n", "type", arg.saves.definition.type);
                  printf ("\t\t\t%s: %s\n", "id", arg.saves.definition.id);
                  printf ("\t\t}\n");
                  printf ("\t]\n");
                }  
            }  
        } break;
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
      Token tk = TKGET(tks, i);
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