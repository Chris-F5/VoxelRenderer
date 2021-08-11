#ifndef SWAPCHAIN
#define SWAPCHAIN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"

typedef struct
{
    VkSwapchainKHR swapchain;
    VkExtent2D extent;
    VkFormat format;
    uint32_t imageCount;
    VkImage *images;
    VkImageView *imageViews;
} Swapchain;

Swapchain createSwapchain(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    PhysicalDeviceProperties physicalDeviceProperties,
    GLFWwindow *window,
    VkSurfaceKHR surface);

void cleanupSwapchain(VkDevice device, Swapchain swapchain);

#endif