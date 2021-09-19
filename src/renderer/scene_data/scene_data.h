#ifndef SCENE_DATA
#define SCENE_DATA

#include <stdio.h>

#include <cglm/types.h>
#include <vulkan/vulkan.h>

extern const uint32_t VOX_BLOCK_SCALE;
extern const uint32_t VOX_BLOCK_VOX_COUNT;

typedef char Voxel;

typedef struct {
    mat4 model;
} BlockDescriptorUniformBuffer;

typedef struct {
    uint32_t maxBlockCount;
    uint32_t maxPaletteCount;

    uint32_t allocatedBlocks;
    uint32_t allocatedPalettes;

    Voxel* blockVoxels;
    uint32_t* blockPaletteIds;

    vec3* palettes;

    VkBuffer blocksInfoBuffer;
    VkDeviceMemory blocksInfoBufferMemory;

    VkDescriptorPool blocksDescriptorPool;
    VkDescriptorSetLayout blocksDescriptorSetLayout;
    VkDescriptorSet* blockDescriptorSets;

    VkBuffer* vertexBuffers;
    VkDeviceMemory* vertexBuffersMemory;
    uint32_t* vertexBuffersLength;
} SceneData;

SceneData createSceneData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t maxBlockCount,
    uint32_t maxPaletteCount);

uint32_t createPalette(
    SceneData* sceneData,
    FILE* paletteFile);

uint32_t createBlock(
    SceneData* sceneData,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    vec3 pos,
    uint32_t paletteId,
    FILE* blockFile);

void cleanupSceneData(VkDevice device, SceneData sceneData);

#endif
