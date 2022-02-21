#include "sparse_vox_object_loader.h"

#include <stdbool.h>
#include <stdlib.h>

#include "vk_utils/buffer.h"

static bool loadChunk(
    VkDevice logicalDevice,
    ChunkStorage* chunkStorage,
    ChunkStorageChanges* chunkStorageChanges,
    ChunkGpuStorage* chunkGpuStorage,
    FILE* file)
{
    if (feof(file))
        return false;
    unsigned int cx, cy, cz;
    if (fscanf(file, "CHUNK %u %u %u\n", &cx, &cy, &cz) != 3) {
        puts("Error reading sparse voxel object file chunk position. Exiting.");
        exit(EXIT_FAILURE);
    }
    ivec3 chunkPos;
    chunkPos[0] = cx;
    chunkPos[1] = cy;
    chunkPos[2] = cz;

    ChunkRef chunk;
    if (ChunkStorage_findChunkFromPos(chunkStorage, chunkPos, &chunk)) {
        puts("Warning: overwriting chunk when loading svo.");
        ChunkStorageChanges_addColorChanges(
            chunkStorageChanges,
            1,
            &chunk);
        ChunkStorageChanges_addVoxBitMaskChanges(
            chunkStorageChanges,
            1,
            &chunk);
    } else {
        ChunkStorage_add(
            chunkStorage,
            chunkStorageChanges,
            1,
            &chunkPos,
            &chunk);
    }
    uint8_t* chunkBitMask = ChunkStorage_chunkBitMask(chunkStorage, chunk);
    uint8_t* chunkColorData = ChunkStorage_chunkColorData(chunkStorage, chunk);
    uint8_t* chunkNormalData = malloc(CHUNK_VOX_COUNT);

    uint32_t c = 0;
    while (c < CHUNK_VOX_COUNT) {
        {
            unsigned int skip;
            if (fscanf(file, "-%u\n", &skip) == 1) {
                c += skip;
                continue;
            }
        }
        unsigned int col, normal;
        if (fscanf(file, "%u %u\n", &col, &normal) != 2) {
            puts("Error reading sparse voxel object file vox. Exiting.");
            exit(EXIT_FAILURE);
        }
        setBit(chunkBitMask, c);
        chunkColorData[c] = col;
        chunkNormalData[c] = normal;
        c += 1;
    }

    copyDataToBuffer(
        logicalDevice,
        chunkNormalData,
        chunkGpuStorage->normalsMemory,
        chunk * CHUNK_VOX_COUNT,
        CHUNK_VOX_COUNT);

    free(chunkNormalData);
    return true;
}

void loadChunksFromSparseVoxFile(
    VkDevice logicalDevice,
    ChunkStorage* chunkStorage,
    ChunkGpuStorage* chunkGpuStorage,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette,
    FILE* file)
{
    if (file == NULL) {
        puts("Sparse voxel object file is NULL. Exiting.");
        exit(EXIT_FAILURE);
    }

    unsigned int claimedChunkScale;
    if (fscanf(file, "CHUNK_SCALE=%u\n", &claimedChunkScale) != 1) {
        puts("Failed to read chunk size from sparse voxel object file. Exiting.");
        exit(EXIT_FAILURE);
    }
    if (claimedChunkScale != CHUNK_SCALE) {
        puts("Chunk scale of sparse voxel object file does not match renderer"
             "chunk scale. Exiting");
        exit(EXIT_FAILURE);
    }

    vec3* paletteColorData
        = VoxPaletteStorage_getColorData(paletteStorage, palette);
    unsigned int r, g, b;
    for (int i = 0; i < 256; i++) {
        if (fscanf(file, "%u %u %u\n", &r, &g, &b) != 3) {
            puts("Failed to read sparse voxel object file color palette. Exiting");
            exit(EXIT_FAILURE);
        }
        paletteColorData[i][0] = (float)r / 255.0f;
        paletteColorData[i][1] = (float)g / 255.0f;
        paletteColorData[i][2] = (float)b / 255.0f;
    }

    ChunkStorageChanges chunkStorageChanges;
    ChunkStorageChanges_init(&chunkStorageChanges, 512, 512, 512);
    while (loadChunk(logicalDevice, chunkStorage, &chunkStorageChanges, chunkGpuStorage, file))
        ;

    ChunkGpuStorage_update(
        chunkGpuStorage,
        logicalDevice,
        chunkStorage,
        &chunkStorageChanges);

    ChunkStorageChanges_destroy(&chunkStorageChanges);
}
