#ifndef DEVICE
#define DEVICE

#include <stdbool.h>

#include <vulkan/vulkan.h>

typedef struct
{
    uint32_t graphicsFamilyIndex;
    uint32_t presentFamilyIndex;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;

} PhysicalDeviceProperties;

bool checkValidationLayerSupport(void);
VkInstance createInstance(char *appName, uint32_t appVersion, uint32_t vulkanApiVersion, bool validationLayersEnabled);

void sellectPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice *physicalDevice,
    PhysicalDeviceProperties *physicalDeviceProperties);

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, PhysicalDeviceProperties physicalDeviceProperties, bool validationLayersEnabled);

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

#endif
