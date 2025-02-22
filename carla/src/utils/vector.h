#include <stdio.h>
#ifndef VECTOR_H
# define VECTOR_H
# include <stdio.h>
# include <string.h>
# define VECTOR_CONTAINS_INTEGER(type, pVector, field, value, pResult)         \
  do {                                                                         \
    size_t i = 0;                                                              \
    *(pResult) = 0;                                                            \
    for(; i < (pVector)->length; i++ ) {                                       \
      int current = (int)(((type*)(pVector)->items)[i].field);                 \
      if ( current == value ) {                                                \
        *(pResult) = 1;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)
# define VECTOR_CONTAINS(type, pVector, field, value, pResult)                 \
  do {                                                                         \
    size_t i = 0;                                                              \
    *(pResult) = 0;                                                            \
    for(; i < (pVector)->length; i++ ) {                                       \
      char *current = ((type*)(pVector)->items)[i].field;                      \
      if( current == NULL )                                                    \
        continue;                                                              \
      if (strcmp (current, (value)) == 0) {                                    \
        *(pResult) = 1;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)
# define VECTOR_FIND(type, pVector, field, value, pResult)                     \
  do {                                                                         \
    size_t i = 0;                                                              \
    for(; i < (pVector)->length; i++ ) {                                       \
      char *current = ((type*)(pVector)->items)[i].field;                      \
      if( strcmp (current, (value)) == 0 ) {                                   \
        *(pResult) = ((type*)(pVector)->items)[i];                             \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)
# define VECTOR_FIND_AND(type, pVector, field, value, field2, value2, pResult) \
  do {                                                                         \
    size_t i = 0;                                                              \
    for(; i < (pVector)->length; i++ ) {                                       \
      type x = ((type*)(pVector)->items)[i];                                   \
      char *y = x.field;                                                       \
      char *z = x.field2;                                                      \
      if( strcmp (y, (value)) == 0 && strcmp (z, (value2)) == 0 ) {            \
        *(pResult) = x;                                                        \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)
# define VECTOR_FIND_INTEGER(type, pVector, field, value, pResult)             \
  do {                                                                         \
    size_t i = 0;                                                              \
    for(; i < (pVector)->length; i++ ) {                                       \
      int current = (int)(((type*)(pVector)->items)[i].field);                 \
      if( current == value ) {                                                 \
        *(pResult) = ((type*)(pVector)->items)[i];                             \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (0)


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
