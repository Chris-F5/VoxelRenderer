#ifndef BLOCK
#define BLOCK

#include <vulkan/vulkan.h>

void createIndexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t *indexCount,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory);

#endif
