#ifndef STRINGS_H
# define STRINGS_H
# include <stdio.h>
  typedef struct {
    FILE *file;
    int estado; 
    char *buffer; 
    size_t buffer_size;
  } FileManager;

  char *substr(char *str, int start, int length);
  char startsWith(char *str, char *prefix);
  void fprintf_start(FILE *file, char *str);
#endif 