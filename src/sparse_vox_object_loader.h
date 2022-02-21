#ifndef SPARSE_VOX_OBJECT_LOADER_H
#define SPARSE_VOX_OBJECT_LOADER_H

#include <stdio.h>

#include <vulkan/vulkan.h>

#include "chunks.h"

void loadChunksFromSparseVoxFile(
    VkDevice logicalDevice,
    ChunkStorage* chunkStorage,
    ChunkGpuStorage* chunkGpuStorage,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette,
    FILE* file);

#endif
