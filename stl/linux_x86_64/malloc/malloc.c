#include <stdio.h>

void* malloc(size_t size) {
    void* ptr = sbrk(size);
    if (ptr == (void*)-1) {
        return NULL;
    }
    return ptr;
}
