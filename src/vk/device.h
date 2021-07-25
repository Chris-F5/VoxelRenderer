#ifndef DEVICE
#define DEVICE

#include <stdbool.h>

#include <vulkan/vulkan.h>

bool checkValidationLayerSupport(void);
VkInstance createInstance(void);
VkPhysicalDevice sellectPhysicalDevice(VkInstance instance);
uint32_t findGraphicsQueueFamily(VkPhysicalDevice physicalDevice);
VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamilyIndex);

#endif