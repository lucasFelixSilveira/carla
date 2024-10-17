#include <string.h>
#include <stdlib.h>
#include "../utils/vector.h"
#include "../utils/symbols.h"
#include "parser.h"
#include "lexer.h"

#define GET(vector, index) ((Token*)vector->items)[index]

/*
  Expressions
  |
  |- Type -> Definition
  |     |- Identifier -> Optional[=]
  |     |           |- =
  |     |           |- ;
  |     |           |- , <- NEED BE A LAMBDA ARGUMENT
  |
  |- Keyword -> Magic
  |        |- Expression -> Boolean
  |
  |- Expression -> (FunctionCall | Expression) 
  |
  |- /[(](\w|\W)*[{]$/ -> Lambda 
  |                            |- Definitions (arguments)
  |                            |            |- Begin  
*/

void 
tGenerate(Vector *tree, Vector *tks) 
{
  char lambda = 0;
  int i = 0;
  for(; i < tks->length; i++ ) 
    {
      Token first = GET(tks, i);
      switch(first.type)
        {
          case Type:
          case Identifier:
            {
              if( first.type == Type || isType (first.buffer) )
                {
                  i++;
                  Token id = GET(tks, i);
                  if( id.type == Identifier ) 
                    {
                      i++;
                      char *final = GET(tks, i).buffer;
                      if(
                          strcmp (final, "=") == 0 ||
                          strcmp (final, ";") == 0 ||
                          (
                            (strcmp (final, ",") == 0 || strcmp (final, ")") == 0) 
                            && lambda
                          ) 
                        ) {
                            vector_push (tree, ((void*)&(PNode) {
                              .type = NODE_DEFINITION,
                              .data = {
                                .definition = {
                                  .hopeful = strcmp (final, "=") == 0,
                                  .id      = id.buffer,
                                  .type    = first.buffer
                                }
                              }
                            }));
                          }
                      else
                        goto __cancel_parse__;
                    }
                }
            } break;
          case Unknown:
            {
              if( strcmp (first.buffer, "}") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_END,
                    .data = {
                      .number = 0
                    }
                  }));
                }

              if( strcmp (first.buffer, "{") == 0 && lambda )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_BEGIN,
                    .data = {
                      .number = 0
                    }
                  }));
                  lambda = 0;
                }
                
             

              if( strcmp (first.buffer, "(") == 0 )
                {
                  int j = i + 1;
                  while( strcmp (GET(tks, j++).buffer, ")") != 0 );
                  if( strcmp (GET(tks, j).buffer, "{") == 0 )
                    {
                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_LAMBDA,
                        .data = {
                          .number = 0
                        }
                      }));
                      lambda = 1;
                      break;
                    } 
                }
            }
          default: break;
        }
    }
  goto skip;
  __cancel_parse__: {
    printf ("\nPARSE ERROR: Fail! Check your code\n");
  }
  skip: {}
}