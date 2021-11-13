#include "./vert_gen.h"

#include <stdlib.h>
#include <stdio.h>

#include <cglm/vec3.h>

#include "./vox_blocks.h"

#define MAX_BLOCK_VERT_COUNT (VOX_BLOCK_VOX_COUNT * 18)

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

static inline uint32_t xyxToBlockIndex(int x, int y, int z)
{
    return x + y * VOX_BLOCK_SCALE + z * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;
}

void generateVoxBlockVertices(
    VkDevice logicalDevice,
    const vec3* colorPalette,
    const unsigned char* blockColors,
    ModelStorage* modelStorage,
    ModelRef targetModel)
{
    uint32_t vertCount = 0;
    ModelVertex* verts
        = (ModelVertex*)malloc(MAX_BLOCK_VERT_COUNT * sizeof(ModelVertex));
    for (int i = 0; i < VOX_BLOCK_VOX_COUNT; i++)
        if (blockColors[i] != 0) {
            int x = i % VOX_BLOCK_SCALE;
            int y = i / VOX_BLOCK_SCALE % VOX_BLOCK_SCALE;
            int z = i / (VOX_BLOCK_SCALE * VOX_BLOCK_SCALE);

            vec3 color;
            color[0] = colorPalette[blockColors[i]][0];
            color[1] = colorPalette[blockColors[i]][1];
            color[2] = colorPalette[blockColors[i]][2];

            if (x == 0
                || blockColors[xyxToBlockIndex(x - 1, y, z)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_XN) / sizeof(FACE_POINTS_XN[0]),
                    FACE_POINTS_XN,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_XN) / sizeof(FACE_POINTS_XN[0]);
            }
            if (x == VOX_BLOCK_SCALE - 1
                || blockColors[xyxToBlockIndex(x + 1, y, z)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_XP) / sizeof(FACE_POINTS_XP[0]),
                    FACE_POINTS_XP,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_XP) / sizeof(FACE_POINTS_XP[0]);
            }

            if (y == 0
                || blockColors[xyxToBlockIndex(x, y - 1, z)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_YN) / sizeof(FACE_POINTS_YN[0]),
                    FACE_POINTS_YN,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_YN) / sizeof(FACE_POINTS_YN[0]);
            }
            if (y == VOX_BLOCK_SCALE - 1
                || blockColors[xyxToBlockIndex(x, y + 1, z)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_YP) / sizeof(FACE_POINTS_YP[0]),
                    FACE_POINTS_YP,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_YP) / sizeof(FACE_POINTS_YP[0]);
            }

            if (z == 0
                || blockColors[xyxToBlockIndex(x, y, z - 1)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_ZN) / sizeof(FACE_POINTS_ZN[0]),
                    FACE_POINTS_ZN,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_ZN) / sizeof(FACE_POINTS_ZN[0]);
            }
            if (z == VOX_BLOCK_SCALE - 1
                || blockColors[xyxToBlockIndex(x, y, z + 1)] == 0) {
                insertVerts(
                    x, y, z,
                    color,
                    sizeof(FACE_POINTS_ZP) / sizeof(FACE_POINTS_ZP[0]),
                    FACE_POINTS_ZP,
                    &verts[vertCount]);
                vertCount += sizeof(FACE_POINTS_ZP) / sizeof(FACE_POINTS_ZP[0]);
            }
        }

    ModelStorage_updateVertexData(
        modelStorage,
        logicalDevice,
        targetModel,
        vertCount,
        verts);
    free(verts);
}
