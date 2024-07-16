#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector 
vector_init (size_t unitySize) 
{
  return (Vector) {
    .items = NULL,
    .length = 0,
    .unity = unitySize
  };
}

void 
vector_push (Vector *pVector, void *item) 
{
  size_t itemSize = pVector->unity;
  pVector->length++;
  void *nItems = realloc (pVector->items, pVector->length * itemSize);
  if( nItems == NULL ) 
    {
      fprintf (stderr, "Fail to alloc memory.\n");
      exit (EXIT_FAILURE);
    }
  pVector->items = nItems;
  memcpy ((char *)pVector->items + (pVector->length - 1) * itemSize, item, itemSize);
}
