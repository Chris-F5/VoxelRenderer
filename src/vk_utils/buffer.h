#ifndef BUFFER_H
#define BUFFER_H

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

#endif
