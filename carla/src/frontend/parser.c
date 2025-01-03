#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../utils/vector.h"
#include "../utils/symbols.h"
#include "../utils/carla.h"
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
trim_non_alphanumeric(char *str) 
{
  int i, j = 0;
  
  for(
    i = 0;
    str[i] != '\0'; 
    i++
  ) {
      if (isalnum((unsigned char) str[i])) 
        str[j++] = str[i];
    }
  str[j] = '\0'; 
}

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
                CASE("#include")
                  Token quoute = GET(tks, ++i);
                  if( quoute.type == Text )
                    {
                      int result;
                      VECTOR_CONTAINS(Library, libs, lib, quoute.buffer, &result);
                      if( result ) 
                        continue;

                      char *lib = strdup (quoute.buffer);
                      trim_non_alphanumeric (lib);

                      vector_push (libs, ((void*)&(Library) {
                        .lib = lib,
                        .fn  = NULL,
                        .is  = 0 
                      }));
                      
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

              if( first.type == Keyword && strcmp (first.buffer, "return") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_RET,
                    .data = {
                      .number = 0
                    }
                  }));
                  break;
                }
              
              if( first.type == Keyword && strcmp (first.buffer, "for") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_FOR,
                    .data = {
                      .number = 0
                    }
                  }));
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
                          strcmp (final, ":")    == 0 ||
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
                                  .hopeful  = strcmp (final, "=") == 0,
                                  .iter     = strcmp (final, ":") == 0,
                                  .type     = first.buffer,
                                  .id       = id.buffer
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



              Token next = GET(tks, i + 1);
              Token after = GET(tks, i + 2);
              Token parenthesis = GET(tks, i + 3);
              if(
                (
                  first.type == Identifier 
                  || (
                    first.type == Keyword
                    && strcmp (first.buffer, "super") == 0
                  ) 
                ) 
                && next.type == Quad && after.type == Identifier && parenthesis.type == Unknown && parenthesis.buffer[0] == '(' 
              ) {
                  i += 3;

                  if( strcmp (first.buffer, "super") == 0 )
                    {
                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_INTERNAL_SUPER,
                        .data.super = after.buffer
                      }));
                      break;
                    }

                  char *module = (char*)malloc (128);
                  sprintf (module, "std%s", first.buffer);
                  int result;
                  VECTOR_CONTAINS(Library, libs, lib, module, &result);

                  if( result )
                    {
                      Library library;
                      VECTOR_FIND(Library, libs, lib, module, &library);
                      VECTOR_CONTAINS(Library, libs, fn, after.buffer, &result);

                      if(! result )
                        {
                          vector_push (libs, ((void*)&(Library) {
                            .fn  = strdup (after.buffer),
                            .lib = library.lib,
                            .is  = 1
                          }));
                        }

                      vector_push (tree, ((void*)&(PNode) {
                        .type = NODE_INTERNAL,
                        .data = {
                          .internal   = {
                            .function = after.buffer,
                            .lib      = module
                          }
                        }
                      }));

                      continue;
                    } 
                  else
                    {
                      free (module);
                      goto __cancel_parse__;
                    }
                }

              // EXPRESSIONS
              if( 
                  ( first.type    == Integer || first.type == Identifier || first.type == Text    ) 
                  && ( next.type  == MathOP  || next.type  == Semi       || next.type  == Unknown || next.type == Iter || next.type == BitsOP || next.type == ComparationOP )
              ) {
                  if(
                      ( next.type == Semi ) 
                      || (
                          next.type == Unknown 
                          && (
                            next.buffer[0] == ']' 
                            || next.buffer[0] == '}'
                            || next.buffer[0] == ')'
                            || next.buffer[0] == ','
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

                        if( next.type == Semi )
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_EEXPR,
                            .data = {
                              .number = 0
                            }
                          }));

                        break;
                      }
                    else 
                      if( 
                           ( first.type == Integer || first.type == Identifier )
                        && ( next.type  == MathOP  || next.type  == BitsOP || next.type == ComparationOP || next.type == Iter )
                        && ( first.type == Integer || first.type == Identifier )
                      ) 
                        {
                          i += 2;
                          vector_push (tree, ((void*)&(PNode) {
                            .type = NODE_OPERATION,
                            .data = {
                              .operation   = {
                                .left      = first,
                                .operation = next,
                                .right     = after, 
                              }
                            }
                          }));
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

          case Semi:
            {
              vector_push (tree, ((void*)&(PNode) {
                .type = NODE_EEXPR,
                .data = {
                  .number = 0
                }
              }));
            }

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

              if( strcmp (first.buffer, ")") == 0 )
                {
                  vector_push (tree, ((void*)&(PNode) {
                    .type = NODE_CLOSE,
                    .data = {
                      .number = 0
                    }
                  }));
                }

              if( strcmp (first.buffer, "{") == 0 )
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