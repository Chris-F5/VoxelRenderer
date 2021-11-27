#ifndef VERT_GEN_H
#define VERT_GEN_H

#include <vulkan/vulkan.h>

#include "./models.h"
#include "./chunks.h"

typedef struct {
    uint32_t vertCount;
    ModelVertex* vertBuffer;
} ChunkVertGen;

void ChunkVertGet_init(ChunkVertGen* vertGen);

void ChunkVertGen_generate(
    ChunkVertGen* vertGen,
    ChunkStorage* chunkStorage,
    ChunkRef chunk,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette);

void ChunkVertGen_destroy(ChunkVertGen* vertGen);

#endif
