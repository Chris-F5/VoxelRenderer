#ifndef PHYSICAL_DEVICE_MEMORY_H
#define PHYSICAL_DEVICE_MEMORY_H

#include <vulkan/vulkan.h>

uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t memoryTypeBits,
    VkMemoryPropertyFlags properties);

#endif
