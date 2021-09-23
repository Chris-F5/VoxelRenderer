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

typedef struct {
    uint32_t blockId;
    PaletteRef paletteRef;
} BlockRef;

typedef struct {
    mat4 model;
} BlockDescriptorUniformBuffer;

typedef struct {
    uint32_t maxBlockCount;
    uint32_t maxPaletteCount;

    uint32_t allocatedBlocks;
    uint32_t allocatedPalettes;

    Voxel* blockVoxels;

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

void createBlock(
    SceneData* sceneData,
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    vec3 pos,
    PaletteRef paletteRef,
    FILE* blockFile,
    bool* created,
    BlockRef* blockRef);

void cleanupSceneData(VkDevice device, SceneData sceneData);

#endif
