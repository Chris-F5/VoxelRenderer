#ifndef DEVICE
#define DEVICE

#include <stdbool.h>

#include <vulkan/vulkan.h>

typedef struct
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilies;

bool checkValidationLayerSupport(void);
VkInstance createInstance(void);

void sellectPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice *physicalDevice,
    QueueFamilies *queueFamilies);

QueueFamilies findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilies queueFamilies);

#endif