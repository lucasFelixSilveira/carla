#include <stdio.h>
#ifndef VECTOR_H
# define VECTOR_H
  typedef struct {
    size_t unity;
    size_t length;
    void *items;
  } Vector;

  Vector vector_init(size_t unitySize);
  void vector_push(Vector *pVector, void *item);
  void vector_remove(Vector *pVector, int index);
  void vector_free(Vector *pVector);
#endif
