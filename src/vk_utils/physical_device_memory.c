#include "./physical_device_memory.h"

#include <stdio.h>
#include <stdlib.h>

uint32_t findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t memoryTypeBits,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (size_t i = 0; i < memProperties.memoryTypeCount; i++)
        if ((memoryTypeBits & (1 << i))
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    puts("Exiting because failed to find requested memory type");
    exit(EXIT_FAILURE);
}
