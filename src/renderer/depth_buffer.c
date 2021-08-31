#include "depth_buffer.h"

#include "vk_utils/image.h"

void createDepthBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkFormat depthImageFormat,
    VkExtent2D extent,
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemroy,
    VkImageView* depthImageView)
{
    VkExtent3D extent3D;
    extent3D.width = extent.width;
    extent3D.height = extent.height;
    extent3D.depth = 1;

    createImage(
        device,
        physicalDevice,
        VK_IMAGE_TYPE_2D,
        depthImageFormat,
        extent3D,
        0,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        false,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemroy);


    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    *depthImageView = createImageView(
        device,
        *depthImage,
        depthImageFormat,
        VK_IMAGE_VIEW_TYPE_2D,
        subresourceRange);
}
