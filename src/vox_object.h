#ifndef VOX_OBJECT_H
#define VOX_OBJECT_H

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include <cglm/types.h>

#include "./vox_blocks.h"

typedef struct {
    vec3 pos;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    VoxPaletteRef palette;
    VoxBlockRef* blocks;
    ModelRef* models;
    bool* blockMask;
} VoxObject;

void VoxObject_init(
    VoxObject* object,
    VoxBlockStorage* blockStorage,
    vec3 pos,
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    VoxPaletteRef palette);

void VoxObject_setVoxel(
    VoxObject* object,
    VkDevice logicalDevice,
    VoxBlockStorage* blockStorage,
    ModelStorage* modelStorage,
    ivec3 pos,
    unsigned char voxColor);

bool VoxObject_getBlock(
    const VoxObject* object,
    int x,
    int y,
    int z,
    VoxBlockRef* blockRef);

void VoxObject_destroy(
    VoxObject* object,
    VoxBlockStorage* blockStorage);

#endif
