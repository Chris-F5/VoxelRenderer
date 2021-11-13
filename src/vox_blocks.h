#ifndef VOX_BLOCKS_H
#define VOX_BLOCKS_H

#include <stdint.h>

#include <cglm/types.h>

#include "id_allocator.h"

extern const int VOX_BLOCK_SCALE;
extern const int VOX_BLOCK_VOX_COUNT;

typedef unsigned int VoxBlockRef;
typedef unsigned int VoxPaletteRef;

typedef struct {
    IdAllocator idAllocator;
    vec3* colors;
} VoxPaletteStorage;

typedef struct {
    IdAllocator idAllocator;
    unsigned char* colors;
} VoxBlockStorage;

void VoxPaletteStorage_init(VoxPaletteStorage* storage);
VoxPaletteRef VoxPaletteStorage_add(VoxPaletteStorage* storage);
vec3* VoxPaletteStorage_getColorData(VoxPaletteStorage* storage, VoxPaletteRef id);
void VoxPaletteStorage_remove(VoxPaletteStorage* storage, VoxPaletteRef id);
void VoxPaletteStorage_destroy(VoxPaletteStorage* storage);

void VoxBlockStorage_init(VoxBlockStorage* storage);
VoxBlockRef VoxBlockStorage_add(VoxBlockStorage* storage);
unsigned char* VoxBlockStorage_getColorData(VoxBlockStorage* storage, VoxBlockRef id);
void VoxBlockStorage_remove(VoxBlockStorage* storage, VoxBlockRef id);
void VoxBlockStorage_destroy(VoxBlockStorage* storage);

#endif
