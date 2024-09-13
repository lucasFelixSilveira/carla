#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "vector.h"
#include "symbols.h"

#ifndef _WIN32
# include <wctype.h>
#endif


char buffer[2048];
size_t len = 0;

TokenType 
valid_buffer () 
{
  if( isKeyword (buffer) ) return Keyword;
  else if( isIdentifier (buffer) ) return Identifier;
  else if( isNumeric (buffer) ) return Integer;
  return Unknown;
}

char 
ismatch (char c1, char c2)
{
  if( c1 == '=' && c2 == '=' ) 
    return Eq;
  else if( c1 == ':' && c2 == '=' ) 
    return Walrus;
  else if( c1 == ':' && c2 == ':' ) 
    return Quad;
  else if( c1 == '-' && c2 == '>' ) 
    return Acess;
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
  else if( c1 == '.' && c2 == '.' ) 
    return Iter;
  else
    return 0;
}

char 
ishexa(char c) 
{
  char *hex = "0123456789abcdefABCDEF";
  for(
    int i = 0;
    i < 16 + 6;
    i++
  ) {
      if( hex[i] == c ) 
        /*->*/ return 1;
    }

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

      char j = getc (file);
      if( c == '-' && j == '-' )
        {
          while(getc (file) != '\n')
            {}
          continue;
        }
      else 
        /*->*/ ungetc (j, file);

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


        if( c == '"' ) {
          char *strbuff = (char*)malloc (1024);
          size_t i = 0;
          size_t j = 1;
          char ch;
          while( (ch = getc (file)) != '"' ) {
            j++;
            if( ch == '\\' ) {
              strbuff[i++] = ch; 
              char __ch2 = getc (file);
              if( ishexa (__ch2) ) {
                strbuff[i++] = __ch2; 
                __ch2 = getc (file);
                if(! ishexa (__ch2) ) {
                  ungetc (__ch2, file);
                  continue;
                } 
                strbuff[i++] = __ch2; 
                continue;
              } else {
                ungetc (__ch2, file);
                continue;
              }
            }
            strbuff[i++] = ch;
          }
          strbuff[i] = 0; 

          vector_push (tks, (void*)(&(Token) {
            .buffer = strdup (strbuff), 
            .type = Literal,
            .real = j
          }));

          free (strbuff);
          continue;
        }

        char c2 = getc (file);
        char t = 1;
        int id = ismatch (c, c2); 
        if( id == 0 ) {
          ungetc (c2, file);
          buffer[0] = c;
        } else {
          buffer[0] = c;
          buffer[1] = c2;
          t++;
        } 

        buffer[t] = 0;

        vector_push (tks, (void*)(&(Token) {
          .buffer = strdup (buffer), 
          .type = (id == 0 ? Unknown : id),
          .real = 0
        }));
      } else {
        buffer[len++] = c;
        buffer[len] = 0;
      }
    }
  
  vector_push (tks, (void*)(&(Token) {
    .buffer = "", 
    .type = EndOfFile,
    .real = 0
  }));
}