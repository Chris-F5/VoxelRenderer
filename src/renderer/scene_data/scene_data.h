#ifndef SCENE_DATA
#define SCENE_DATA

#include <stdbool.h>
#include <stdio.h>

#include <cglm/types.h>
#include <vulkan/vulkan.h>

extern const uint32_t VOX_BLOCK_SCALE;
extern const uint32_t VOX_BLOCK_VOX_COUNT;

typedef unsigned char Voxel;
typedef uint32_t PaletteRef;
typedef uint32_t BlockRef;

typedef struct {
    mat4 model;
} BlockDescriptorUniformBuffer;

typedef struct {
    uint32_t maxBlockCount;
    uint32_t maxPaletteCount;

    uint32_t allocatedBlocks;
    uint32_t allocatedPalettes;

    Voxel* blockVoxels;

    vec3* paletteColors;

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

PaletteRef createPalette(
    SceneData* sceneData,
    FILE* paletteFile);

BlockRef createEmptyBlock(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    PaletteRef paletteRef);

BlockRef createBlockFromFile(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    PaletteRef paletteRef,
    FILE* blockFile);

Voxel* getBlockVoxels(
    SceneData* sceneData,
    BlockRef block);

void updateBlockVertexBuffer(
    VkDevice device,
    SceneData* sceneData,
    PaletteRef palette,
    BlockRef block);

void cleanupSceneData(VkDevice device, SceneData sceneData);

#endif
