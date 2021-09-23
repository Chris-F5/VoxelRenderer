#include "object.h"

#include <stdlib.h>
#include <string.h>

Object createEmptyObject(
    SceneData* sceneData,
    uint32_t width,
    uint32_t height,
    uint32_t depth)
{
    Object object;
    object.width = width;
    object.height = height;
    object.depth = depth;
    object.blocks = (BlockRef*)malloc(
        width * height * depth * sizeof(BlockRef));
    object.blocksMask = (bool*)malloc(
        width * height * depth * sizeof(bool));
    memset(object.blocksMask, 0, width * height * depth * sizeof(bool));

    return object;
}

Object createObject(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    FILE* objectFile)
{
    uint32_t dimensions[3];
    fread(&dimensions, sizeof(uint32_t), 3, objectFile);
    Object object = createEmptyObject(
        sceneData,
        dimensions[0],
        dimensions[1],
        dimensions[2]);

    PaletteRef palette = createPalette(sceneData, objectFile);

    for (uint32_t i = 0; i < object.width * object.height * object.depth; i++) {
        int x = i % object.width;
        int y = i / object.width % object.height;
        int z = i / (object.width * object.height);
        vec3 pos = { x * VOX_BLOCK_SCALE, y * VOX_BLOCK_SCALE, z * VOX_BLOCK_SCALE };

        bool created;
        BlockRef block;
        createBlock(
            sceneData,
            device,
            physicalDevice,
            pos,
            palette,
            objectFile,
            &created,
            &block);
        object.blocksMask[i] = created;
        if (created)
            object.blocks[i] = block;
    }

    return object;
}

void cleanupObject(SceneData* sceneData, Object object)
{
    for (uint32_t i = 0; i < object.width * object.height * object.depth; i++)
        if (object.blocksMask[i])
            continue; // TODO: cleanup block in scene data
    free(object.blocks);
    free(object.blocksMask);
}
