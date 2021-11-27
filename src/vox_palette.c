#include "./vox_palette.h"

#include <stdlib.h>
#include <string.h>

static const unsigned int PALETTE_CAPACITY = 100;

void VoxPaletteStorage_init(VoxPaletteStorage* storage)
{
    IdAllocator_init(&storage->idAllocator, PALETTE_CAPACITY);
    storage->colors
        = (vec3*)malloc(PALETTE_CAPACITY * 256 * sizeof(vec3));
}

VoxPaletteRef VoxPaletteStorage_add(VoxPaletteStorage* storage)
{
    unsigned int id;
    IdAllocator_allocate(&storage->idAllocator, 1, &id);
    memset(&storage->colors[id * 256], 0, 256);
    return id;
}

vec3* VoxPaletteStorage_getColorData(VoxPaletteStorage* storage, VoxPaletteRef id)
{
    return &storage->colors[id * 256];
}

void VoxPaletteStorage_remove(VoxPaletteStorage* storage, VoxPaletteRef id)
{
    IdAllocator_remove(&storage->idAllocator, 1, &id);
}

void VoxPaletteStorage_destroy(VoxPaletteStorage* storage)
{
    IdAllocator_destroy(&storage->idAllocator);
    free(storage->colors);
}
