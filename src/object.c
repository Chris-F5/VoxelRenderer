#include "object.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

Object createEmptyObject(
    SceneData* sceneData,
    vec3 pos,
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    PaletteRef palette)
{
    Object object;
    object.pos[0] = pos[0];
    object.pos[1] = pos[1];
    object.pos[2] = pos[2];
    object.width = width;
    object.height = height;
    object.depth = depth;
    object.palette = palette;
    object.blocks = (BlockRef*)malloc(
        width * height * depth * sizeof(BlockRef));
    object.blocksMask = (bool*)malloc(
        width * height * depth * sizeof(bool));
    memset(object.blocksMask, 0, width * height * depth * sizeof(bool));

    return object;
}

Object createObjectFromFile(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    FILE* objectFile)
{
    if(objectFile == NULL){
        puts("Oject file is NULL. Exiting.");
        exit(EXIT_FAILURE);
    }

    uint32_t dimensions[3];
    fread(&dimensions, sizeof(uint32_t), 3, objectFile);

    PaletteRef palette = createPaletteFromFile(sceneData, objectFile);

    Object object = createEmptyObject(
        sceneData,
        pos,
        dimensions[0],
        dimensions[1],
        dimensions[2],
        palette);

    for (uint32_t i = 0; i < object.width * object.height * object.depth; i++) {
        ivec3 blockPos;
        blockPos[0] = i % object.width;
        blockPos[1] = i / object.width % object.height;
        blockPos[2] = i / (object.width * object.height);

        vec3 worldPos;
        worldPos[0] = blockPos[0] * VOX_BLOCK_SCALE + object.pos[0];
        worldPos[1] = blockPos[1] * VOX_BLOCK_SCALE + object.pos[1];
        worldPos[2] = blockPos[2] * VOX_BLOCK_SCALE + object.pos[2];

        // TODO: dont create empty blocks
        object.blocksMask[i] = true;
        object.blocks[i] = createBlockFromFile(
            device,
            physicalDevice,
            sceneData,
            worldPos,
            palette,
            objectFile);
    }

    return object;
}

void setObjectVoxel(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    Object* object,
    ivec3 pos,
    Voxel vox)
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

    if (!object->blocksMask[blockId]) {
        vec3 blockWorldPos;
        blockWorldPos[0] = blockPos[0] * VOX_BLOCK_SCALE + object->pos[0];
        blockWorldPos[1] = blockPos[1] * VOX_BLOCK_SCALE + object->pos[1];
        blockWorldPos[2] = blockPos[2] * VOX_BLOCK_SCALE + object->pos[2];
        object->blocksMask[blockId] = true;
        object->blocks[blockId] = createEmptyBlock(
            device,
            physicalDevice,
            sceneData,
            blockWorldPos,
            object->palette);
    }
    getBlockVoxels(sceneData, object->blocks[blockId])[voxId] = vox;
}

void updateObjectVertexBuffers(
    VkDevice device,
    SceneData* sceneData,
    Object object)
{
    for (int b = 0; b < object.width * object.height * object.depth; b++)
        if (object.blocksMask[b])
            updateBlockVertexBuffer(
                device,
                sceneData,
                object.palette,
                object.blocks[b]);
}

void cleanupObject(SceneData* sceneData, Object object)
{
    for (uint32_t i = 0; i < object.width * object.height * object.depth; i++)
        if (object.blocksMask[i])
            continue; // TODO: cleanup block in scene data
    free(object.blocks);
    free(object.blocksMask);
}
