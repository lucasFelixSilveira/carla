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
    }
  pVector->items = nItems;
  memcpy ((char*)pVector->items + (pVector->length - 1) * itemSize, item, itemSize);
}

void 
vector_remove(Vector *pVector, int index)
{
  if( index >= pVector->length ) 
    return;

  size_t itemSize = pVector->unity;
  size_t i = index; 
  for (; i < pVector->length - 1; ++i )
    { 
      memcpy ((char*)pVector->items + i * itemSize, (char*)pVector->items + (i + 1) * itemSize, itemSize);
    }

  pVector->length--;
  pVector->items = realloc (pVector->items, pVector->length * itemSize);
}

void 
vector_free(Vector *pVector)
{
  free (pVector->items);
  pVector->items = NULL;
  pVector->length = 0;
}