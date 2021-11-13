#ifndef INDEX_ALLOCATOR_H
#define INDEX_ALLOCATOR_H

#include <stdbool.h>

typedef struct {
    unsigned int capacity;
    unsigned int maskFilled;
    bool* mask;
} IdAllocator;

void IdAllocator_init(IdAllocator* allocator, unsigned int capacity);

unsigned int IdAllocator_add(IdAllocator* allocator);

void IdAllocator_remove(IdAllocator* allocator, unsigned int id);

void IdAllocator_destroy(IdAllocator* allocator);

#endif
