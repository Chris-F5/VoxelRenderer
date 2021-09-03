#ifndef DESCRIPTOR_SET
#define DESCRIPTOR_SET

#include <cglm/types.h>
#include <vulkan/vulkan.h>

typedef struct {
    mat4 view;
    mat4 proj;
} GlobalUniformBuffer;

typedef struct {
    mat4 model;
} MeshUniformBuffer;

void createDescriptorSetLayouts(
    VkDevice device,
    VkDescriptorSetLayout* globalDescriptorSetLayout,
    VkDescriptorSetLayout* meshDescriptorSetLayout);

void createGlobalDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout layout,
    uint32_t count,
    VkDescriptorPool* descriptorPool,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory);

void createMeshDescriptorPool(
    VkDevice device,
    uint32_t maxMeshCount,
    VkDescriptorPool* descriptorPool);

void createMeshDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout layout,
    uint32_t count,
    VkDescriptorPool descriptorPool,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory);

#endif
