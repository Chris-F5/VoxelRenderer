#include "./id_allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void IdAllocator_init(IdAllocator* allocator, uint32_t capacity)
{
    allocator->count = 0;
    allocator->capacity = capacity;
    allocator->maskFilled = 0;
    allocator->mask = (bool*)malloc(capacity * sizeof(bool));
    memset(allocator->mask, 0, capacity * sizeof(bool));
}

void IdAllocator_allocate(
    IdAllocator* allocator,
    uint32_t count,
    uint32_t* ids)
{
    allocator->count += count;
    uint32_t idsAllocated = 0;
    uint32_t i = 0;
    for (; i < allocator->maskFilled; i++)
        if (allocator->mask[i] == false) {
            ids[idsAllocated] = i;
            allocator->mask[i] = true;
            idsAllocated++;
            if (idsAllocated == count)
                return;
        }
    for (; idsAllocated < count; i++) {
        ids[idsAllocated] = i;
        allocator->mask[i] = true;
        idsAllocated++;
    }
    if (i >= allocator->maskFilled)
        allocator->maskFilled = i + 1;
    if (allocator->maskFilled > allocator->capacity) {
        puts("Failed to add item to id allocator. Reached max count.");
        exit(EXIT_FAILURE);
    }
}

void IdAllocator_remove(
    IdAllocator* allocator,
    uint32_t count,
    uint32_t* ids)
{
    allocator->count -= count;
    for (uint32_t i = 0; i < count; i++)
        allocator->mask[ids[i]] = false;
    while (allocator->maskFilled > 0
        && allocator->mask[allocator->maskFilled - 1] == false) {
        allocator->maskFilled -= 1;
    }
}

bool IdAllocator_first(IdAllocator* allocator, uint32_t* id)
{
    for (uint32_t i = 0; i < allocator->maskFilled; i++)
        if (allocator->mask[i] == true) {
            *id = i;
            return true;
        }
    return false;
}

uint32_t IdAllocator_next(IdAllocator* allocator, uint32_t lastId, uint32_t* id)
{
    for (uint32_t i = lastId + 1; i < allocator->maskFilled; i++)
        if (allocator->mask[i] == true) {
            *id = i;
            return true;
        }
    return false;
}

void IdAllocator_destroy(IdAllocator* allocator)
{
    free(allocator->mask);
}
