#include "./vox_object.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void VoxObject_init(
    VoxObject* object,
    VoxBlockStorage* blockStorage,
    vec3 pos,
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    VoxPaletteRef palette)
{
    object->pos[0] = pos[0];
    object->pos[1] = pos[1];
    object->pos[2] = pos[2];
    object->width = width;
    object->height = height;
    object->depth = depth;
    object->palette = palette;
    object->blocks = (VoxBlockRef*)malloc(
        width * height * depth * sizeof(VoxBlockRef));
    object->models = (ModelRef*)malloc(
        width * height * depth * sizeof(ModelRef));
    object->blockMask = (bool*)malloc(
        width * height * depth * sizeof(bool));
    memset(object->blockMask, 0, width * height * depth * sizeof(bool));
}

void VoxObject_setVoxel(
    VoxObject* object,
    VkDevice logicalDevice,
    VoxBlockStorage* blockStorage,
    ModelStorage* modelStorage,
    ivec3 pos,
    unsigned char voxColor)
{
    ivec3 blockPos;
    blockPos[0] = (int)floorf(pos[0] / (float)VOX_BLOCK_SCALE);
    blockPos[1] = (int)floorf(pos[1] / (float)VOX_BLOCK_SCALE);
    blockPos[2] = (int)floorf(pos[2] / (float)VOX_BLOCK_SCALE);

    if (blockPos[0] < 0
        || blockPos[0] >= object->width
        || blockPos[1] < 0
        || blockPos[1] >= object->height
        || blockPos[2] < 0
        || blockPos[2] >= object->depth) {
        printf(
            "Can not insert voxel into position (%d, %d, %d) of object with size (%d, %d, %d). Exiting.\n",
            pos[0], pos[1], pos[2],
            object->width * VOX_BLOCK_SCALE,
            object->height * VOX_BLOCK_SCALE,
            object->depth * VOX_BLOCK_SCALE);
        exit(EXIT_FAILURE);
    }

    uint32_t blockId
        = blockPos[0]
        + blockPos[1] * object->width
        + blockPos[2] * object->width * object->height;
    uint32_t voxId
        = pos[0] % VOX_BLOCK_SCALE
        + pos[1] % VOX_BLOCK_SCALE * VOX_BLOCK_SCALE
        + pos[2] % VOX_BLOCK_SCALE * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;

    if (!object->blockMask[blockId]) {
        vec3 blockWorldPos;
        blockWorldPos[0] = blockPos[0] * VOX_BLOCK_SCALE + object->pos[0];
        blockWorldPos[1] = blockPos[1] * VOX_BLOCK_SCALE + object->pos[1];
        blockWorldPos[2] = blockPos[2] * VOX_BLOCK_SCALE + object->pos[2];
        object->blockMask[blockId] = true;
        object->blocks[blockId] = VoxBlockStorage_add(blockStorage);
        object->models[blockId] = ModelStorage_add(
            modelStorage,
            logicalDevice,
            50000);
    }
    VoxBlockStorage_getColorData(
        blockStorage,
        object->blocks[blockId])[voxId]
        = voxColor;
}

bool VoxObject_getBlock(
    const VoxObject* object,
    int x,
    int y,
    int z,
    VoxBlockRef* blockRef)
{
    uint32_t blockId
        = x
        + y * object->width
        + z * object->width * object->height;

    if (object->blockMask[blockId]) {
        *blockRef = object->blocks[blockId];
        return true;
    } else
        return false;
}

void VoxObject_destroy(
    VoxObject* object,
    VoxBlockStorage* blockStorage)
{
    for (uint32_t i = 0; i < object->width * object->height * object->depth; i++)
        if (object->blockMask[i])
            VoxBlockStorage_remove(blockStorage, object->blocks[i]);
    free(object->blocks);
    free(object->blockMask);
}
