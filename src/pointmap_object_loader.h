#ifndef POINTMAP_OBJECT_LOADER_H
#define POINTMAP_OBJECT_LOADER_H

#include "./chunks.h"
#include "./vox_palette.h"
#include <stdio.h>

void loadChunksFromPointmapFile(
    ChunkStorage* chunkStorage,
    ChunkStorageChanges* chunkStorageChanges,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette,
    FILE* file);

#endif
