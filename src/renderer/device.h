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
    VkFormat depthBufferFormat;
} PhysicalDeviceProperties;

bool checkValidationLayerSupport(void);

VkInstance createInstance(char* appName, uint32_t appVersion, uint32_t vulkanApiVersion, bool validationLayersEnabled);

void selectPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice* physicalDevice,
    PhysicalDeviceProperties* physicalDeviceProperties);

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, PhysicalDeviceProperties physicalDeviceProperties, bool validationLayersEnabled);

#endif
