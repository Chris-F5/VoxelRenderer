#include "./vert_gen.h"

#include <stdio.h>
#include <stdlib.h>

#include <cglm/vec3.h>

#include "./bit_array.h"
#include "./vk_utils/exceptions.h"

#define MAX_CHUNK_VERT_COUNT (CHUNK_VOX_COUNT * 18)

const vec3 FACE_POINTS_XP[] = {
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 1 },

    { 1, 1, 0 },
    { 1, 1, 1 },
    { 1, 0, 1 }
};
const vec3 FACE_POINTS_XN[] = {
    { 0, 0, 0 },
    { 0, 0, 1 },
    { 0, 1, 0 },

    { 0, 1, 0 },
    { 0, 0, 1 },
    { 0, 1, 1 }
};
const vec3 FACE_POINTS_YP[] = {
    { 0, 1, 0 },
    { 0, 1, 1 },
    { 1, 1, 0 },

    { 1, 1, 0 },
    { 0, 1, 1 },
    { 1, 1, 1 }
};
const vec3 FACE_POINTS_YN[] = {
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 1 },

    { 1, 0, 0 },
    { 1, 0, 1 },
    { 0, 0, 1 }
};
const vec3 FACE_POINTS_ZP[] = {
    { 0, 0, 1 },
    { 1, 0, 1 },
    { 0, 1, 1 },

    { 1, 0, 1 },
    { 1, 1, 1 },
    { 0, 1, 1 }
};
const vec3 FACE_POINTS_ZN[] = {
    { 0, 0, 0 },
    { 0, 1, 0 },
    { 1, 0, 0 },

    { 1, 0, 0 },
    { 0, 1, 0 },
    { 1, 1, 0 }
};

static void insertVerts(
    int xOffset,
    int yOffset,
    int zOffset,
    const vec3 color,
    size_t pointCount,
    const vec3* points,
    ModelVertex* targetVertsLocation)
{
    for (int i = 0; i < pointCount; i++) {
        targetVertsLocation[i].pos[0] = points[i][0] + (float)xOffset;
        targetVertsLocation[i].pos[1] = points[i][1] + (float)yOffset;
        targetVertsLocation[i].pos[2] = points[i][2] + (float)zOffset;

        targetVertsLocation[i].color[0] = color[0];
        targetVertsLocation[i].color[1] = color[1];
        targetVertsLocation[i].color[2] = color[2];
    }
}

static inline uint32_t xyzToChunkIndex(int x, int y, int z)
{
    return x + y * CHUNK_SCALE + z * CHUNK_SCALE * CHUNK_SCALE;
}

void ChunkVertGen_init(ChunkVertGen* vertGen)
{
    vertGen->vertCount = 0;
    vertGen->vertBuffer
        = (ModelVertex*)malloc(MAX_CHUNK_VERT_COUNT * sizeof(ModelVertex));
}

void ChunkVertGen_generate(ChunkVertGen* vertGen,
    ChunkStorage* chunkStorage,
    ChunkGpuStorage* chunkGpuStorage,
    VkDevice logicalDevice,
    ChunkRef chunk,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette)
{
    uint8_t* chunkBitMask = ChunkStorage_chunkBitMask(chunkStorage, chunk);
    uint8_t* chunkColors = ChunkStorage_chunkColorData(chunkStorage, chunk);
    vec3* colorPalette = VoxPaletteStorage_getColorData(paletteStorage, palette);
    uint32_t* chunkBrightness;

    handleVkResult(
        vkMapMemory(
            logicalDevice,
            chunkGpuStorage->brightnessMemory,
            chunk * CHUNK_VOX_COUNT * sizeof(uint32_t),
            CHUNK_VOX_COUNT,
            0,
            (void**)&chunkBrightness),
        "mapping brightness gpu memory for vert gen");

    vertGen->vertCount = 0;
    for (int i = 0; i < CHUNK_VOX_COUNT; i++)
        if (testBit(chunkBitMask, i) != 0) {
            int x = i % CHUNK_SCALE;
            int y = i / CHUNK_SCALE % CHUNK_SCALE;
            int z = i / (CHUNK_SCALE * CHUNK_SCALE);

            uint32_t brightness = chunkBrightness[i];
            vec3 color;
            color[0] = colorPalette[chunkColors[i]][0]
                * ((float)brightness / (float)UINT16_MAX);
            color[1] = colorPalette[chunkColors[i]][1]
                * ((float)brightness / (float)UINT16_MAX);
            color[2] = colorPalette[chunkColors[i]][2]
                * ((float)brightness / (float)UINT16_MAX);

            if (x == 0
                || testBit(chunkBitMask, xyzToChunkIndex(x - 1, y, z)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_XN) / sizeof(FACE_POINTS_XN[0]),
                    FACE_POINTS_XN,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_XN) / sizeof(FACE_POINTS_XN[0]);
            }
            if (x == CHUNK_SCALE - 1
                || testBit(chunkBitMask, xyzToChunkIndex(x + 1, y, z)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_XP) / sizeof(FACE_POINTS_XP[0]),
                    FACE_POINTS_XP,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_XP) / sizeof(FACE_POINTS_XP[0]);
            }

            if (y == 0
                || testBit(chunkBitMask, xyzToChunkIndex(x, y - 1, z)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_YN) / sizeof(FACE_POINTS_YN[0]),
                    FACE_POINTS_YN,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_YN) / sizeof(FACE_POINTS_YN[0]);
            }
            if (y == CHUNK_SCALE - 1
                || testBit(chunkBitMask, xyzToChunkIndex(x, y + 1, z)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_YP) / sizeof(FACE_POINTS_YP[0]),
                    FACE_POINTS_YP,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_YP) / sizeof(FACE_POINTS_YP[0]);
            }

            if (z == 0
                || testBit(chunkBitMask, xyzToChunkIndex(x, y, z - 1)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_ZN) / sizeof(FACE_POINTS_ZN[0]),
                    FACE_POINTS_ZN,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_ZN) / sizeof(FACE_POINTS_ZN[0]);
            }
            if (z == CHUNK_SCALE - 1
                || testBit(chunkBitMask, xyzToChunkIndex(x, y, z + 1)) == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_ZP) / sizeof(FACE_POINTS_ZP[0]),
                    FACE_POINTS_ZP,
                    &vertGen->vertBuffer[vertGen->vertCount]);
                vertGen->vertCount
                    += sizeof(FACE_POINTS_ZP) / sizeof(FACE_POINTS_ZP[0]);
            }
        }

    vkUnmapMemory(
        logicalDevice,
        chunkGpuStorage->brightnessMemory);
}

void ChunkVertGen_destroy(ChunkVertGen* vertGen)
{
    free(vertGen->vertBuffer);
}
