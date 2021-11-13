#include "./vox_blocks.h"

#include <stdlib.h>
#include <string.h>

const int VOX_BLOCK_SCALE = 32;
const int VOX_BLOCK_VOX_COUNT = VOX_BLOCK_SCALE * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;

static const unsigned int BLOCK_CAPACITY = 100;
static const unsigned int PALETTE_CAPACITY = 100;

void VoxPaletteStorage_init(VoxPaletteStorage* storage)
{
    IdAllocator_init(&storage->idAllocator, PALETTE_CAPACITY);
    storage->colors
        = (vec3*)malloc(PALETTE_CAPACITY * 256 * sizeof(vec3));
}

VoxPaletteRef VoxPaletteStorage_add(VoxPaletteStorage* storage)
{
    unsigned int id = IdAllocator_add(&storage->idAllocator);
    memset(&storage->colors[id * 256], 0, VOX_BLOCK_VOX_COUNT);
    return id;
}

vec3* VoxPaletteStorage_getColorData(VoxPaletteStorage* storage, VoxPaletteRef id)
{
    return &storage->colors[id * 256];
}

void VoxPaletteStorage_remove(VoxPaletteStorage* storage, VoxPaletteRef id)
{
    IdAllocator_remove(&storage->idAllocator, id);
}

void VoxPaletteStorage_destroy(VoxPaletteStorage* storage)
{
    IdAllocator_destroy(&storage->idAllocator);
    free(storage->colors);
}

void VoxBlockStorage_init(VoxBlockStorage* storage)
{
    IdAllocator_init(&storage->idAllocator, BLOCK_CAPACITY);
    storage->colors
        = (uint8_t*)malloc(BLOCK_CAPACITY * VOX_BLOCK_VOX_COUNT * sizeof(uint8_t));
}

VoxBlockRef VoxBlockStorage_add(VoxBlockStorage* storage)
{
    unsigned int id = IdAllocator_add(&storage->idAllocator);
    memset(&storage->colors[id * VOX_BLOCK_VOX_COUNT], 0, VOX_BLOCK_VOX_COUNT);
    return id;
}

unsigned char* VoxBlockStorage_getColorData(VoxBlockStorage* storage, VoxBlockRef id)
{
    return &storage->colors[id * VOX_BLOCK_VOX_COUNT * sizeof(unsigned char)];
}

void VoxBlockStorage_remove(VoxBlockStorage* storage, VoxBlockRef id)
{
    IdAllocator_remove(&storage->idAllocator, id);
}

void VoxBlockStorage_destroy(VoxBlockStorage* storage)
{
    IdAllocator_destroy(&storage->idAllocator);
    free(storage->colors);
}
