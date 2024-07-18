#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"

const Key staticSymbols[KEYWORDS_LENGTH + TYPES_LENGTH]
  = { 
    { .len = 6, .val = "return" },
    { .len = 2, .val = "if" },
    { .len = 4, .val = "else" },
    /* types */
    { .len = 3, .val = "int" },
    { .len = 4, .val = "char" },
    { .len = 5, .val = "short" },
    { .len = 4, .val = "long" },
    { .len = 4, .val = "half" },
    { .len = 5, .val = "float" },
    { .len = 6, .val = "double" },
    { .len = 4, .val = "void" }
  };

char 
inStaticSymbols (char buffer[], int x, int y) 
{
  size_t length = strlen (buffer);
  for(
    int i = x;
    i < y;
    i++
  ) {
      Key word = staticSymbols[i];

      if( word.len != length ) 
        /*->*/ continue;

      if( strcmp (buffer, word.val) == 0 )
        /*->*/ return 1;
    }
  return 0;
}

/* Checks if the:
  [a-zA-Z@][a-zA-Z0-9]
REGEX is match */
char
isIdentifier (char buffer[])
{
  char first = buffer[0];
  if( first != '@' && !isalpha (first) ) 
    /*->*/ return 0;
  
  for(
    int i = 0;
    i < (strlen (buffer) - 1);
    i++
  ) {
      if(! isalnum (buffer[i]) )
        /*->*/ return 0;
    }

  return 1;
}

char
isKeyword (char buffer[])
{ return inStaticSymbols (buffer, 0, KEYWORDS_LENGTH); }

char
isType (char buffer[])
{ return inStaticSymbols (buffer, KEYWORDS_LENGTH - 1, KEYWORDS_LENGTH + TYPES_LENGTH); }