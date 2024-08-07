#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "vector.h"
#include "symbols.h"

#define TKGET(i) ((Token*)tks->items)[i] 
#define ISOP(n) n > OP_CONSTANT
#define POS(x, y) x + y
#define EQ(var, constant) strcmp(var, constant) == 0
#define CH(tk) tk.buffer[0]

typedef enum { false = 0, true = 1 } bool;
int type_len = 0;
char *types[4096];

char *
clone(Token tk) 
{
  return strdup (tk.buffer);
}

void 
pGenerate (Vector *root, Vector *tks) 
{
  Scope current = Global; 

  int i = 0;
  while( i < tks->length ) 
    {
      if( CH(TKGET(i)) == 0xffffffff ) 
        {
          i++;
          continue;
        }

      int old = i;
      /*
        Identifier:
          -> Identifier(Type)    |
            + Identifier(id)     | <- Definition expression
            + Operator(=) | SEMI |
      */

      if( 
        ( 
          (
            TKGET(i).type == Identifier 
            && isTypeStack (TKGET(i).buffer, (StackType) { .types = types, .length = type_len }) 
            && (tks->length >= (i + 2))
          ) || EQ(TKGET(i).buffer, "type")
        )
        /* Checks whether the next token is a valid identifier for a definition */
        && isIdentifier (TKGET(POS(i, 1)).buffer) 
        /* Checks if the next token is a value assignment operator or a semi */
        && ( 
          EQ(TKGET(POS(i, 2)).buffer, "=") 
          + EQ(TKGET(POS(i, 2)).buffer, ";") 
          + EQ(TKGET(POS(i, 2)).buffer, ",") 
        ) > 0
      ) {   
          Token type = TKGET(i++);
          Token id = TKGET(i++);

          if( EQ(TKGET(i - 2).buffer, "type") ) 
            {
              types[type_len++] = strdup (id.buffer);
            }

          vector_push (root, (void*)(&(PNode) {
            .scope = current,
            .type = Definition,
            .saves = (Cache) {
              .definition = (DMemory) {
                .hopeful = EQ(TKGET(i).buffer, "="),
                .id = clone (id), .type = clone (type),
                .key_type = EQ(TKGET(i - 2).buffer, "type"),
                .arg = EQ(TKGET(i).buffer, ",") || EQ(TKGET(i).buffer, ")"),
                .array = NULL
              }
            }
          }));
          i++;
        }
      
      else
      if( TKGET(i).type == Keyword ) 
        {
          vector_push (root, (void*)(&(PNode) {
            .scope = current,
            .type = Magic,
            .saves = (Cache) {
              .magic = clone (TKGET(i++))
            }
          }));
        }
      else
      if( EQ(TKGET(i).buffer, "(") ) 
        {

          if( EQ(TKGET(POS(i,1)).buffer, "void") && EQ(TKGET(POS(i,2)).buffer, ")") && EQ(TKGET(POS(i,3)).buffer, "{") ) 
            /*->*/ goto only_add;

          if( 
            (
              TKGET(POS(i,1)).type == Integer
              || EQ(TKGET(POS(i,1)).buffer, "_")
            ) 
            && EQ(TKGET(POS(i,2)).buffer, ":")
            && TKGET(POS(i,3)).type == Identifier
            && EQ(TKGET(POS(i,4)).buffer, ")")
          ) {
              if( TKGET(POS(i,5)).type == Identifier )
                {
                  vector_push (root, (void*)(&(PNode) {
                    .scope = current,
                    .type = Definition,
                    .saves = (Cache) {
                      .definition = (DMemory) {
                        .hopeful = EQ(TKGET(i).buffer, "="),
                        .id = clone (TKGET(POS(i,5))), .type = "ptr", .key_type = 0,
                        .arg = EQ(TKGET(POS(i,6)).buffer, ",") || EQ(TKGET(POS(i,6)).buffer, ")"),
                        .array = (AMemory) {
                          .size = TKGET(POS(i,1)).type == Integer ? TKGET(POS(i,1)).buffer : "undefined",
                          .type = TKGET(POS(i,3)).buffer
                        }
                      }
                    }
                  }));

                  i += EQ(TKGET(POS(i,6)).buffer, ",") || EQ(TKGET(POS(i,6)).buffer, ")") ? 2 : 1;
                }
              else
                {
                  vector_push (root, (void*)(&(PNode) {
                    .scope = current,
                    .type = ArrayType,
                    .saves = (Cache) { 
                      .array = (AMemory) {
                        .size = TKGET(POS(i,1)).type == Integer ? TKGET(POS(i,1)).buffer : "undefined",
                        .type = TKGET(POS(i,3)).buffer
                      }
                    }
                  }));
                }

              i += 5;
              continue;
            }

          int x = 1;
          goto jump_steps;
next_parenthesis: {}
x++;
jump_steps: {}

          while( 
            EQ(TKGET(POS(i, x)).buffer, ")") == 0 
          ) { x++; }
          
          if( EQ(TKGET(POS(i, POS(x,1))).buffer, "{") )
            {
only_add: {}
              vector_push (root, (void*)(&(PNode) {
                .scope = current,
                .type = Lambda,
                .saves = (Cache) { .nothing = 0 }
              }));
            }
          else 
            /*->*/ goto next_parenthesis;
          i += 1;
        }
      else
      if( EQ(TKGET(i).buffer, "{") || EQ(TKGET(i).buffer, "}") ) 
        {
          vector_push (root, (void*)(&(PNode) {
            .scope = current,
            .type = EQ(TKGET(i).buffer, "{") ? Begin : End,
            .saves = (Cache) { .nothing = 0 }
          }));
          
          i++;
        }  
      else 
      if( EQ(TKGET(i).buffer, ";") ) 
        { i++; }
      else 
        { 
          vector_push (root, (void*)(&(PNode) {
            .scope = current,
            .type = Normal,
            .saves = (Cache) { .token = TKGET(i++) }
          }));
        };

      /*
        Expressions
      */
      // else
      // if( TKGET(i).type == Identifier ) 
      //   {
      //     int start, end = i;
      //     for(
      //       ; end < tks->length;
      //       end++
      //     ) {

      //       }
      //   }
    }
}