#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>

#include <vulkan/vulkan.h>

void createImage(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkImageType imageType,
    VkFormat format,
    VkExtent3D extent,
    VkImageCreateFlags flags,
    VkImageUsageFlags usageFlags,
    uint32_t mipLevels,
    uint32_t arrayLayers,
    VkSampleCountFlagBits samples,
    VkImageTiling tiling,
    bool preinitialized,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkImage* image,
    VkDeviceMemory* imageMemory);

void createImageView(
    VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageViewType viewType,
    VkImageSubresourceRange imageSubresourceRange,
    VkImageView* imageView);

#endif
