#include "./id_allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void IdAllocator_init(IdAllocator* allocator, unsigned int capacity)
{
    allocator->capacity = capacity;
    allocator->maskFilled = 0;
    allocator->mask = (bool*)malloc(capacity * sizeof(bool));
    memset(allocator->mask, 0, capacity * sizeof(bool));
}

unsigned int IdAllocator_add(IdAllocator* allocator)
{
    unsigned int id = allocator->maskFilled;
    for (int i = 0; i < allocator->maskFilled; i++)
        if (allocator->mask[i] == false) {
            id = i;
            break;
        }
    if (id == allocator->maskFilled)
        allocator->maskFilled += 1;
    if (id >= allocator->capacity) {
        puts("Failed to add item to id allocator. Reached max count.");
        exit(EXIT_FAILURE);
    }
    allocator->mask[id] = true;

    return id;
}

void IdAllocator_remove(IdAllocator* allocator, unsigned int id)
{
    allocator->mask[id] = false;
    while (allocator->maskFilled > 0
        && allocator->mask[allocator->maskFilled - 1] == false) {
        allocator->maskFilled -= 1;
    }
}

void IdAllocator_destroy(IdAllocator* allocator)
{
    free(allocator->mask);
}
