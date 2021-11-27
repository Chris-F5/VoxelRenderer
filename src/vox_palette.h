#ifndef VOX_PALETTE_H
#define VOX_PALETTE_H

#include <cglm/types.h>

#include "./id_allocator.h"

typedef unsigned int VoxPaletteRef;

typedef struct {
    IdAllocator idAllocator;
    vec3* colors;
} VoxPaletteStorage;

void VoxPaletteStorage_init(VoxPaletteStorage* storage);
VoxPaletteRef VoxPaletteStorage_add(VoxPaletteStorage* storage);
vec3* VoxPaletteStorage_getColorData(VoxPaletteStorage* storage, VoxPaletteRef id);
void VoxPaletteStorage_remove(VoxPaletteStorage* storage, VoxPaletteRef id);
void VoxPaletteStorage_destroy(VoxPaletteStorage* storage);

#endif

