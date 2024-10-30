#include <string.h>
#include <stdlib.h>
#include "../utils/vector.h"
#include "../utils/symbols.h"
#include "../utils/strings.h"
#include "parser.h"
#include "lexer.h"

#define GET(vector, index) ((Token*)vector->items)[index]

char
types_int(char *type)
{
  size_t i = KEYWORDS_LENGTH + MACROS_LENGTH + (TYPES_LENGTH - INTEGER_TYPES_LENGTH);
  for(; i < TOTAL; i++)
    if( strcmp (staticSymbols[i].val, type) == 0 )
      return 1;
  return 0;
}

void 
parseType(Vector *lex, Vector *tks) 
{

  char lambda = 0;
  int i = 0;
  for(; i < lex->length; i++ ) 
    {
      Token first = GET(lex, i);
      switch(first.type)
        {
          case Identifier:
            {
              char *type = (char*)malloc (128);
              type[0] = 0;
              sprintf (type, "%s", first.buffer);
              Token next;
              int ptr = 0;
              while(1) {
                next = GET(lex, i+1);
                if( next.type == MathOP && strcmp (next.buffer, "*") == 0 ) {
                  sprintf (type, "%s*", type);
                  ptr++;
                  i++;
                  continue;
                }

                if( ptr > 0 ) {
                  vector_push (tks, ((void*)&(Token) {
                    .buffer = strdup (type),
                    .type = Type
                  }));
                }
                break;
              }
              free (type);
              if( ptr > 0 )
                break;
            } 
          case Unknown:
            {
              int success = 0;
              if( strcmp (first.buffer, "[") == 0 && (GET(lex, i - 1).type == Unknown || GET(lex, i - 1).type == Semi || GET(lex, i - 1).type == Keyword) )
                {
                  char *type = (char*)malloc (255);
                  type[0] = 0;
                  i++;
                  while(1) {
                    Token next = GET(lex, i);
                    if( next.type == Integer || strcmp (next.buffer, "]") == 0 ) {
                      int old = strcmp (next.buffer, "]") == 0;
                      char **buff;
                      if( next.type == Integer ) {
                        char *d = strdup (next.buffer);
                        buff = &d;
                      }
                      if( old )
                        *buff = "";
                      else
                        next = GET(lex, ++i);
                      if( old || strcmp (next.buffer, "]") == 0 ) {
                        sprintf (type, "%s[%s]", type, *buff);
                        next = GET(lex, ++i);
                        if( strcmp (next.buffer, "[") == 0 ) {
                          continue;
                        } else if( next.type == Identifier ) {
                          sprintf (type, "%s%s", type, next.buffer);
                          while(1) {
                            next = GET(lex, ++i);
                            if( next.type == MathOP && strcmp (next.buffer, "*") == 0 )
                              sprintf (type, "%s*", type);
                            else break;
                          }
                          i--;
                          success = 1;
                          vector_push (tks, ((void*)&(Token) {
                            .buffer = type,
                            .type = Type
                          }));
                          break;
                        }
                      }
                    }
                    free (type);
                    goto __cancel_typement;
                  }
                }

              if( success )
                break;
            }
          default: {
            vector_push (tks, ((void*)&(Token) {
              .buffer = first.buffer,
              .type   = first.type,
              .real   = first.real
            }));
          } break;
        }
    }
  goto skip;
  __cancel_typement: {
    printf ("\nInvalid type received\n");
  }
  skip: {}
}