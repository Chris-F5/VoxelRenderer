#include "swapchain.h"

#include <stdlib.h>

#include "vk_utils/exceptions.h"
#include "vk_utils/image.h"

uint32_t min(uint32_t x, uint32_t y)
{
    return (x < y) ? x : y;
}

uint32_t max(uint32_t x, uint32_t y)
{
    return (x > y) ? x : y;
}

VkExtent2D chooseExtent(GLFWwindow* window, VkSurfaceCapabilitiesKHR surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        return surfaceCapabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D extent;

        extent.width = min(
            surfaceCapabilities.maxImageExtent.width,
            max(
                width,
                surfaceCapabilities.maxImageExtent.width));
        extent.height = min(
            surfaceCapabilities.maxImageExtent.height,
            max(
                height,
                surfaceCapabilities.maxImageExtent.height));

        return extent;
    }
}

Swapchain createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    PhysicalDeviceProperties physicalDeviceProperties,
    GLFWwindow* window,
    VkSurfaceKHR surface)
{
    Swapchain swapchain;
    swapchain.extent = chooseExtent(window, physicalDeviceProperties.surfaceCapabilities);
    swapchain.format = physicalDeviceProperties.surfaceFormat.format;
    swapchain.imageCount = physicalDeviceProperties.surfaceCapabilities.minImageCount + 1;
    if (physicalDeviceProperties.surfaceCapabilities.maxImageCount != 0 && swapchain.imageCount > physicalDeviceProperties.surfaceCapabilities.maxImageCount)
        swapchain.imageCount = physicalDeviceProperties.surfaceCapabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = NULL;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = swapchain.imageCount;
    swapchainCreateInfo.imageFormat = physicalDeviceProperties.surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = physicalDeviceProperties.surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = swapchain.extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (physicalDeviceProperties.graphicsFamilyIndex != physicalDeviceProperties.presentFamilyIndex) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        uint32_t queueFamilyIndices[] = { physicalDeviceProperties.graphicsFamilyIndex, physicalDeviceProperties.presentFamilyIndex };
        swapchainCreateInfo.queueFamilyIndexCount = sizeof(queueFamilyIndices) / sizeof(queueFamilyIndices[0]);
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = NULL;
    }
    swapchainCreateInfo.preTransform = physicalDeviceProperties.surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = physicalDeviceProperties.presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    handleVkResult(
        vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain.swapchain),
        "creating swapchain");

    handleVkResult(
        vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, NULL),
        "getting swapchain image count");
    swapchain.images = (VkImage*)malloc(swapchain.imageCount * sizeof(VkImage));
    handleVkResult(
        vkGetSwapchainImagesKHR(device, swapchain.swapchain, &swapchain.imageCount, swapchain.images),
        "getting swapchain images");

    swapchain.imageViews = (VkImageView*)malloc(swapchain.imageCount * sizeof(VkImageView));
    for (int i = 0; i < swapchain.imageCount; i++) {
        VkImageSubresourceRange subresourceRange;
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        swapchain.imageViews[i] = createImageView(
            device,
            swapchain.images[i],
            swapchain.format,
            VK_IMAGE_VIEW_TYPE_2D,
            subresourceRange);
    }

    return swapchain;
}

void cleanupSwapchain(VkDevice device, Swapchain swapchain)
{
    for (int i = 0; i < swapchain.imageCount; i++)
        vkDestroyImageView(device, swapchain.imageViews[i], NULL);

    vkDestroySwapchainKHR(device, swapchain.swapchain, NULL);
    free(swapchain.images);
    free(swapchain.imageViews);
}
