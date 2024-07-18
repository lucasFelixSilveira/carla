#ifndef VECTOR_H
# define VECTOR_H
  typedef struct {
    size_t unity;
    size_t length;
    void *items;
  } Vector;

  Vector vector_init(size_t unitySize);
  void vector_push(Vector *pVector, void *item);
#endif
