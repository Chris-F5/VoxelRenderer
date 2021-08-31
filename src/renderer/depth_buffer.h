#ifndef DEPTH_BUFFER
#define DEPTH_BUFFER

#include <vulkan/vulkan.h>

#include "device.h"

void createDepthBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkFormat depthImageFormat,
    VkExtent2D extent,
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemroy,
    VkImageView* depthImageView);

#endif
