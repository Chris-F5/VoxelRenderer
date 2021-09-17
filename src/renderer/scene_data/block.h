#ifndef BLOCK
#define BLOCK

#include <stdio.h>

#include <cglm/types.h>
#include <vulkan/vulkan.h>

extern const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

extern const uint32_t VOX_BLOCK_SCALE;
extern const uint32_t VOX_BLOCK_VOX_COUNT;

typedef struct {
    vec3 pos;
    vec3 color;
} Vertex;

typedef struct
{
    char colorId;
} Voxel;

typedef struct {
    mat4 model;
} BlockDescriptorUniformBuffer;

typedef struct
{
    Voxel* voxels;
    BlockDescriptorUniformBuffer descriptorData;
    VkDescriptorSet* descriptorSets;
    VkBuffer* descriptorSetUniformBuffers;
    VkDeviceMemory* descriptorSetUniformBuffersMemory;
    uint32_t vertexBufferLength;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} Block;

VkDescriptorSetLayout createBlockDescriptorSetLayout(VkDevice device);

VkDescriptorPool createBlockDescriptorPool(
    VkDevice device,
    uint32_t swapchainImageCount,
    uint32_t maxBlockCount);

Block createBlock(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout blockDescriptorSetLayout,
    VkDescriptorPool descriptorPool,
    uint32_t swapchainImageCount,
    FILE* blockFile);

void updateBlockDescriptors(VkDevice device, Block* block, uint32_t currentSwapchainImageIndex);

void cleanupBlock(
    VkDevice device,
    Block block,
    uint32_t swapchainImageCount,
    VkDescriptorPool blockDescriptorPool);

#endif
