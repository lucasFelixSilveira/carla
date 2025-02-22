#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *substr(const char *str, int start, int length) {
  int str_length = strlen (str);
  
  if( start < 0 || start >= str_length || length <= 0 ) 
    return NULL; 
  
  if( start + length > str_length ) 
    length = str_length - start;

  char *result = (char*)malloc (length + 1); 
  if( result == NULL ) 
    return NULL; 

  strncpy (result, str + start, length);
  result[length] = '\0'; 

  return result;
}

char 
startsWith(char *str, char *prefix) 
{
  if( strlen (prefix) > strlen (str) )
    return 0;

  size_t i = 0;
  for(; i < strlen (prefix); i++)
    {
      if( str[i] != prefix[i] )
        break;
    }

  return i == strlen (prefix);
}