#ifndef GLOBAL_DESCRIPTOR_SET
#define GLOBAL_DESCRIPTOR_SET

#include <cglm/types.h>
#include <vulkan/vulkan.h>

typedef struct {
    mat4 view;
    mat4 proj;
} GlobalUniformBuffer;


VkDescriptorSetLayout createGlobalDescriptorSetLayout(VkDevice device);

void createGlobalDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout layout,
    uint32_t count,
    VkDescriptorPool* descriptorPool,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory);

#endif
