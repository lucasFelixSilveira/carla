#ifndef CARLA_H
# define CARLA_H
# include "vector.h"

  int isCarla(char *__file__);
  void carla_project(char **output_file);

  typedef struct {
    char  is;
    char *lib;
    char *fn;
  } Library;
  
#endif