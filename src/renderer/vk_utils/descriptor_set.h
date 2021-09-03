#ifndef DESCRIPTOR_SET
#define DESCRIPTOR_SET

#include <vulkan/vulkan.h>

void allocateDescriptorSets(
    VkDevice device,
    VkDescriptorSetLayout layout,
    VkDescriptorPool descriptorPool,
    uint32_t count,
    VkDescriptorSet* descriptorSets);

void createUniformBuffers(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    size_t uniformBufferSize,
    uint32_t count,
    VkBuffer* buffers,
    VkDeviceMemory* buffersMemory);

#endif
