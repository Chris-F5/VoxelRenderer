#ifndef BUFFER
#define BUFFER

#include <vulkan/vulkan.h>

void createBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size,
    VkBufferCreateFlags flags,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkBuffer* buffer,
    VkDeviceMemory* bufferMemory);

void copyDataToBuffer(
    VkDevice device,
    const void* data,
    VkDeviceMemory bufferMemory,
    size_t offset,
    size_t size);

void bufferTransfer(
    VkDevice device,
    VkQueue queue,
    VkCommandPool commandPool,
    uint32_t copyRegionCount,
    VkBufferCopy *copyRegions,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer);

#endif
