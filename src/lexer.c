#include <stdio.h>
#include "lexer.h"
#include "vector.h"

char *buffer[2048];

Token *
tokenize (FILE *file, Vector *tks)
{
  while(1)
    {
      if( feof(file) ) 
        /*->*/ break;
      
      // if(  )
    }
}