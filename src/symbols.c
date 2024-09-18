#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "symbols.h"

const Key staticSymbols[KEYWORDS_LENGTH + TYPES_LENGTH]
  = { 
    { .len = 6, .val = "extern" },
    { .len = 6, .val = "return" },
    { .len = 5, .val = "while" },
    { .len = 4, .val = "type" },
    { .len = 4, .val = "else" },
    { .len = 3, .val = "for" },
    { .len = 3, .val = "cut" },
    { .len = 2, .val = "if" },
    /* types */
    { .len = 2, .val = "i1" },
    { .len = 2, .val = "i8" },
    { .len = 2, .val = "u8" },
    { .len = 3, .val = "i16" },
    { .len = 3, .val = "u16" },
    { .len = 3, .val = "i32" },
    { .len = 3, .val = "u32" },
    { .len = 3, .val = "i64" },
    { .len = 3, .val = "u64" },
    { .len = 3, .val = "f32" },
    { .len = 3, .val = "f64" },
    { .len = 4, .val = "void" },
    { .len = 3, .val = "ptr" }
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
  [a-zA-Z@][a-zA-Z0-9]+
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

/* Checks if the:
  [0-9]+
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

char
isTypeStack (char buffer[], StackType stack)
{
  if( isType (buffer) )
    /*->*/ return 1;

  for(
    int i = 0;
    i < stack.length;
    i++
  ) {
      char *type = stack.types[i];
      if( strcmp (type, buffer) == 0 )
        return 1;
    }
  
  return 0;
}