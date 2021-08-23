#ifndef DESCRIPTOR_SET
#define DESCRIPTOR_SET

#include <cglm/types.h>
#include <vulkan/vulkan.h>

typedef struct {
    mat4 model;
    mat4 view;
    mat4 proj;
} UniformBuffer;

void createDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t count,
    VkDescriptorPool* descriptorPool,
    VkDescriptorSetLayout* descriptorSetLayout,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory);

#endif
