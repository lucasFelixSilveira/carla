#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
    size_t unity;
    size_t length;
    void *items;
} Vector;

Vector vector_init(size_t unitySize);
void vector_push(Vector *pVector, void *item, size_t itemSize);

#endif
