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
        TKGET(i).type == Identifier && isType (TKGET(i).buffer) && (tks->length >= (i + 2))
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

          vector_push (root, (void*)(&(PNode) {
            .scope = current,
            .type = Definition,
            .saves = (Cache) {
              .definition = (DMemory) {
                .hopeful = EQ(TKGET(i).buffer, "="),
                .id = clone (id), .type = clone (type),
                .arg = EQ(TKGET(i).buffer, ",") || EQ(TKGET(i).buffer, ")") 
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
          int x = 1;
          while( EQ(TKGET(POS(i, x)).buffer, ")") == 0 ) 
            { x++; }
          
          if( EQ(TKGET(POS(i, POS(x, 1))).buffer, "{") )
            {
              vector_push (root, (void*)(&(PNode) {
                .scope = current,
                .type = Lambda,
                .saves = (Cache) { .nothing = 0 }
              }));
            }
          
          i++;
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