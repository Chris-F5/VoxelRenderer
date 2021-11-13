#include "./debug_utils_extension.h"

#include <stdio.h>

#include "./exceptions.h"

static VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func
        = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT");
    if (func == NULL) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    } else {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
}

static VkResult destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func
        = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");
    if (func == NULL) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    } else {
        func(instance, debugMessenger, pAllocator);
        return VK_SUCCESS;
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    printf("validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

void createDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.messageSeverity
        = /* VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT */
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType
        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = validationLayerCallback;
    createInfo.pUserData = NULL;

    handleVkResult(
        createDebugUtilsMessengerEXT(
            instance,
            &createInfo,
            NULL,
            debugMessenger),
        "creating debug messenger");
}

void destroyDebugMessenger(
    VkInstance instance,
    VkDebugUtilsMessengerEXT* debugMessenger)
{
    handleVkResult(
        destroyDebugUtilsMessengerEXT(
            instance,
            *debugMessenger,
            NULL),
        "destroying debug messenger");
}
