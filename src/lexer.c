#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "vector.h"

char buffer[2048];
size_t len = 0;

#define LARGE_CONSTANT 100000
#define KEYWORDS_LENGTH 3
typedef struct { char len; char *val; } Key;
const Key keywords[KEYWORDS_LENGTH]
    = { 
      { .len = 6, .val = "return" },
      { .len = 4, .val = "root" },
      { .len = 3, .val = "let" }
    };
    
char
isKeyword ()
{
  size_t length = strlen (buffer);
  for(
    int i = 0;
    i < KEYWORDS_LENGTH;
    i++
  ) {
      Key key = keywords[i];

      if( key.len != length ) 
        /*->*/ continue;

      if( strcmp (buffer, key.val) == 0 )
        /*->*/ return 1;
    }
  return 0;
}

char
isIdentifier ()
{
  char first = buffer[0];
  if( first != '@' && !isalpha(first) ) 
    /*->*/ return 0;
  
  for(
    int i = 0;
    i < (strlen(buffer) - 1);
    i++
  ) {
      if(! isalnum(buffer[i]) )
        /*->*/ return 0;
    }

  return 1;
}

TokenType 
valid_buffer () 
{
  if( isKeyword () ) return Keyword;
  else if( isIdentifier () ) return Identifier;
  return Unknown;
}

char 
ismatch (char c1, char c2)
{
  if( c1 == '=' && c2 == '=' ) 
    return Eq;
  else if( c1 == ':' && c2 == '=' ) 
    return Walrus;
  else if( c1 == '+' && c2 == '=' ) 
    return Add;
  else if( c1 == '-' && c2 == '=' ) 
    return Sub;
  else if( c1 == '*' && c2 == '=' ) 
    return Imul;
  else if( c1 == '/' && c2 == '=' ) 
    return Idiv;
  else if( c1 == '!' && c2 == '=' ) 
    return Ne;
  else if( c1 == '>' && c2 == '=' ) 
    return Ge;
  else if( c1 == '<' && c2 == '=' ) 
    return Le;
  else if( c1 == '>' && c2 == '>' ) 
    return RightShift;
  else if( c1 == '<' && c2 == '<' ) 
    return LeftShift;
  else
    return 0;
}

void 
tokenize (FILE *file, Vector *tks)
{
  while(1)
    {
      if( feof (file) ) 
        /*->*/ break;
      
      char c = getc (file);

      if(! isalnum (c) ) {
        if( len > 0 ) {
          vector_push (tks, (void*)(&(Token) {
            .buffer = strdup (buffer), 
            .type = valid_buffer ()
          }));
          len = 0;
        } 

        if( iswspace (c) ) 
          /*->*/ continue;

        char c2 = getc (file);
        char t = 0;
        int id = ismatch (c, c2); 
        if( id == 0 ) {
          ungetc (c2, file);
          buffer[0] = c;
          t = 1;
        } else {
          buffer[0] = c;
          buffer[1] = c2;
          t = 2;
        } 

        buffer[t] = 0;

        vector_push (tks, (void*)(&(Token) {
          .buffer = strdup (buffer), 
          .type = (id == 0 ? Unknown : id)
        }));
      } else {
        buffer[len++] = c;
        buffer[len] = 0;
      }
    }
  
  vector_push (tks, (void*)(&(Token) {
    .buffer = "", 
    .type = EndOfFile
  }));
}