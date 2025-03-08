#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"

char *substr(char *str, int start, int length) {
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

void fprintf_start(FILE *file, char *str) {
  char *buffer = NULL;
  size_t buffer_size = 0;  
  size_t total_read = 0;   

  if (file == NULL) {
      printf("Erro: Arquivo não aberto corretamente.\n");
      return;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
      printf("Erro ao alocar memória para o buffer.\n");
      return;
  }

  total_read = fread(buffer, 1, file_size, file);
  buffer[total_read] = '\0';

  fseek(file, 0, SEEK_SET);
  fprintf(file, "%s }\n", str);
  fprintf(file, "%s", buffer);

  free(buffer);
}