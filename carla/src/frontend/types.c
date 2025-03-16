#include <string.h>
#include <stdlib.h>
#include "../utils/vector.h"
#include "../utils/symbols.h"
#include "../utils/strings.h"
#include "../utils/errors.h"
#include "../utils/json.h"
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
parseType(Vector *lex, Vector *tks, FILE *logs) 
{
  char *failType;
  char *ctx = (char*)malloc(1024);
  char lambda = 0;
  int i = 0;
  Token first;
  for(; i < lex->length; i++ ) 
    {
      first = GET(lex, i);
      switch(first.type)
        {
          case Identifier:
            {
              char *type = (char*)malloc (128);
              type[0] = 0;
              sprintf (type, "%s", first.buffer);
              Token next = GET(lex, i+1);

              if( strcmp (first.buffer, "enum") == 0 && next.type == Identifier ) {
                sprintf (type, "%s %s", type, next.buffer);
                Token colon = GET(lex, i+2);
                Token integer = GET(lex, i+3);
                if( colon.buffer[0] == ':' ) {
                  i += 3;
                  sprintf (type, "%s : %s", type, integer.buffer);
                 
                  if( integer.type != Identifier || !types_int (integer.buffer) ) {
                    failType = strdup (type);
                    sprintf(ctx, "Has been expected an integer type");
                    free(type);
                    goto __cancel_typement;
                  }

                  vector_push (tks, ((void*)&(Token) {
                    .buffer = strdup (type),
                    .def    = next.buffer, 
                    .sub    = integer.buffer, 
                    .type   = EnumType
                  }));
                  continue;
                } 
                else {
                  sprintf(ctx, "Has been expected ';', found '%s'", colon.buffer);
                  failType = strdup (type);
                  free (type);
                  goto __cancel_typement;
                }
              }

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
                    failType = type[0] == 0 ? "[\0" : strdup (type);
                    sprintf(ctx, "Your code open an array and not close it");
                    free(type);
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
              .real   = first.real,
              .local  = first.local
            }));
          } break;
        }
    }
  goto skip;
  __cancel_typement: {
    JPair cPairs[2];
    cPairs[0] = (JPair) { "type", json_string ("error") };
    
    JPair ePairs[4];
    char *msg = (char*)malloc (256);
    sprintf (msg, "`%s...` is an invalid type. : %s", failType, ctx);
    ePairs[0] = (JPair) { "message", json_string (msg) };
    ePairs[1] = (JPair) { "code", json_number (SyntaxTypeError) };
    ePairs[2] = (JPair) { "buffer", json_string (failType) };
    ePairs[3] = (JPair) { "location", json_array ((JValue[]){
      json_number (first.local.posY), json_number (first.local.posX)
    }, 2) };
    cPairs[1] = (JPair) { "error", json_object (ePairs, 4) };

    JValue data = json_object (cPairs, 2);
    char *json;
    json_stringify (&json, data);
    fprintf (logs, "%s", json);
    free (failType);
    exit (1);
  }
  skip: {}
}