#ifndef DEBUG_UTILS_EXTENSION_H
#define DEBUG_UTILS_EXTENSION_H

#include <vulkan/vulkan.h>

void createDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger);

void destroyDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger);

#endif
