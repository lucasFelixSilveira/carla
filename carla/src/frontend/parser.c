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
  |- (EXTERN) ?
  |- | Type -> Definition
  |       |- Identifier -> Optional[=]
  |       |           |- =
  |       |           |- ;
  |       |           |- END <- NEED BE A LIBC FUNCTION
  |       |           |- , <- NEED BE A LAMBDA ARGUMENT
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

#define BEGIN_SWITCH(str)    { char *_switch_str = str;
#define CASE(val)            if( strcmp (_switch_str, (val)) == 0 ) {
#define BREAK_CASE(val)      } else if( strcmp (_switch_str, (val)) == 0 ) {
#define DEFAULT              } else {
#define END_SWITCH           }
#define BREAK                }

void 
tGenerate(Vector *tree, Vector *tks, Vector *libs) 
{
  char lambda = 0;
  int i = 0;
  for(; i < tks->length; i++ ) 
    {
      Token first = GET(tks, i);
      switch(first.type)
        {
          case Macro:
            {
              BEGIN_SWITCH (first.buffer)
                CASE("@import")
                  Token quoute = GET(tks, ++i);
                  if( quoute.type == Text )
                    {
                      char *file = (char*)malloc (strlen (quoute.buffer) + 1);
                      memcpy (file, first.buffer, strlen (first.buffer));
                      vector_push (libs, file);
                    }
                  else
                    goto __cancel_parse__;
                BREAK
              END_SWITCH
            } break;

          case Type:
          case Integer:
          case Text:
          case Keyword:
          case Identifier:
            {

              if( first.type == Keyword && strcmp (first.buffer, "end") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_CUT,
                    .data = {
                      .number = 0
                    }
                  }));
                  lambda = 0;
                  break;
                }

              Token cache = { .buffer = NULL };
              if( first.type == Keyword && strcmp (first.buffer, "extern") == 0 )
                {
                  cache = GET(tks, i);
                  first = GET(tks, ++i);
                }

              if( first.type == Type || isType (first.buffer) )
                {
                  i++;
                  Token id = GET(tks, i);
                  if( id.type == Identifier ) 
                    {
                      i++;
                      char *final = GET(tks, i).buffer;
                      if(
                          strcmp (final, "=")    == 0 ||
                          strcmp (final, ";")    == 0 ||
                          strcmp (final, "end")  == 0 ||
                          (
                            (strcmp (final, ",") == 0 || strcmp (final, ")") == 0) 
                            && lambda
                          ) 
                        ) {
                            vector_push (tree, ((void*)&(PNode) {
                              .type = (cache.buffer == NULL) ? NODE_DEFINITION : NODE_DEF_LIBC,
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
                  else
                    {
                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_TYPE,
                        .data = {
                          .value = first.buffer
                        }
                    }));
                    }
                  break;
                }


              // EXPRESSIONS
              Token next = GET(tks, i + 1);
              if( 
                  ( first.type    == Integer || first.type == Identifier || first.type == Text ) 
                  && ( next.type  == MathOP  || next.type  == Semi       || next.type  == Unknown )
              ) {
                  if(
                      ( next.type == Semi ) 
                      || (
                          next.type == Unknown 
                          && (
                            next.buffer[0] == ']' 
                            || next.buffer[0] == '}'
                            || next.buffer[0] == ')'
                          )
                         )
                    ) { 
                        if( first.type == Identifier ) 
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_ID,
                                .data = {
                                  .value = first.buffer
                                }
                              }
                            }
                          }));
                        else
                        if( first.type == Text ) 
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_TEXT,
                                .data = {
                                  .value = first.buffer
                                }
                              }
                            }
                          }));
                        else
                        if( first.type == Integer ) 
                          printf ("NUMBER: %d", atoi (first.buffer));
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_SINGLE,
                            .data = {
                              .single = {
                                .type = NODE_NUMBER,
                                .data = {
                                  .number = atoi (first.buffer)
                                }
                              }
                            }
                          }));
                        break;
                      }
                }

              Token can  = GET(tks, i + 2);
              if( next.type == Unknown && next.buffer[0] == '[' 
                  && (
                    can.type == Integer || can.type == Identifier
                  ) 
                ) {
                    vector_push (tree, ((void*)&(PNode) {
                      .type = NODE_ACCESS,
                      .data = {
                        .value = first.buffer
                      }
                    }));
                    break;
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