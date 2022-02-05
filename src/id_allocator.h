#ifndef INDEX_ALLOCATOR_H
#define INDEX_ALLOCATOR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t count;
    uint32_t capacity;
    uint32_t maskFilled;
    bool* mask;
} IdAllocator;

void IdAllocator_init(IdAllocator* allocator, uint32_t capacity);

void IdAllocator_allocate(
    IdAllocator* allocator,
    uint32_t count,
    uint32_t* ids);

void IdAllocator_remove(
    IdAllocator* allocator,
    uint32_t count,
    uint32_t* ids);

bool IdAllocator_first(IdAllocator* allocator, uint32_t* id);

uint32_t IdAllocator_next(IdAllocator* allocator, uint32_t lastId, uint32_t* id);

void IdAllocator_destroy(IdAllocator* allocator);

#endif
