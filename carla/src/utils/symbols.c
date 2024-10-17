#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"

const Key staticSymbols[KEYWORDS_LENGTH + TYPES_LENGTH]
  = { 
    { .len = 6, .val = "use" },
    { .len = 6, .val = "return" },
    /* types */
    { .len = 4, .val = "bool" },
    { .len = 4, .val = "int8" },
    { .len = 4, .val = "byte" },
    { .len = 5, .val = "ascii" },
    { .len = 5, .val = "asciz" },
    { .len = 5, .val = "int16" },
    { .len = 5, .val = "int32" },
    { .len = 5, .val = "int64" },
    { .len = 6, .val = "int128" },
    { .len = 5, .val = "float" },
    { .len = 7, .val = "float64" },
    { .len = 4, .val = "void" },
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
        continue;

      if( strcmp (buffer, word.val) == 0 )
        return 1;
    }
  return 0;
}

/* Checks if the:
  [a-zA-Z@][a-zA-Z0-9]+
REGEX is match */
char
isIdentifier (char buffer[])
{
  if( strlen (buffer) > 32 ) 
    return 0;
  char first = buffer[0];
  if( first != '@' && !isalpha (first) ) 
    return 0;
  
  for(
    int i = 0;
    i < (strlen (buffer) - 1);
    i++
  ) {
      if(! isalnum (buffer[i]) )
        return 0;
    }

  return 1;
}

/* Checks if the:
  [0-9]*
REGEX is match */
char
isNumeric (char buffer[])
{
  for(
    int i = 0;
    i < (strlen (buffer) - 1);
    i++
  ) {
      if(! isdigit (buffer[i]) )
        return 0;
    }

  return 1;
}

char
isKeyword (char buffer[])
{ return inStaticSymbols (buffer, 0, KEYWORDS_LENGTH); }

char
isType (char buffer[])
{ return inStaticSymbols (buffer, KEYWORDS_LENGTH - 1, KEYWORDS_LENGTH + TYPES_LENGTH); }