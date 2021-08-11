#ifndef PHYSICAL_DEVICE_MEMORY
#define PHYSICAL_DEVICE_MEMORY

#include <vulkan/vulkan.h>

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

#endif