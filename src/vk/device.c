#include "device.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "exceptions.h"

const uint32_t QUEUE_FAMILY_DOES_NOT_EXIST = UINT32_MAX;

extern char *APP_NAME;
extern uint32_t APP_VERSION;
extern uint32_t VULKAN_API_VERSION;
extern bool validationLayersEnabled;

const char *VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"};

const char *REQUIRED_DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool checkValidationLayerSupport(void)
{
    uint32_t availableLayerCount;
    handleVkResult(
        vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL),
        "getting instance layer properties count to check for validation layer support");
    VkLayerProperties *availableLayers = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * availableLayerCount);
    handleVkResult(
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers),
        "getting instance layer properties to check for validation layer support");

    const int validationLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
    for (int vi = 0; vi < validationLayerCount; vi++)
    {
        bool layerFound = false;
        for (int ai = 0; ai < availableLayerCount; ai++)
            if (strcmp(VALIDATION_LAYERS[vi], availableLayers[ai].layerName) == 0)
            {
                layerFound = true;
                break;
            }

        if (!layerFound)
        {
            free(availableLayers);
            return false;
        }
    }

    free(availableLayers);
    return true;
}

VkInstance createInstance(void)
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = APP_NAME;
    appInfo.applicationVersion = APP_VERSION;
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VULKAN_API_VERSION;

    uint32_t glfwExtensionCount;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if (validationLayersEnabled)
    {
        createInfo.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = NULL;
    }

    VkInstance instance;
    handleVkResult(
        vkCreateInstance(&createInfo, NULL, &instance),
        "creating vk instance");

    return instance;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t availableExtensionCount;
    handleVkResult(
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availableExtensionCount, NULL),
        "getting physical device extension count to check for physical device extension support");
    VkExtensionProperties *availableExtensions =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * availableExtensionCount);
    handleVkResult(
        vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &availableExtensionCount, availableExtensions),
        "getting physical device extensions to check for physical device extension support");

    for (uint32_t ri = 0; ri < sizeof(REQUIRED_DEVICE_EXTENSIONS) / sizeof(REQUIRED_DEVICE_EXTENSIONS[0]); ri++)
    {
        bool layerFound = false;
        for (uint32_t ai = 0; ai < availableExtensionCount; ai++)
            if (strcmp(REQUIRED_DEVICE_EXTENSIONS[ri], availableExtensions[ai].extensionName) == 0)
            {
                layerFound = true;
                break;
            }
        if (!layerFound)
        {
            free(availableExtensions);
            return false;
        }
    }
    free(availableExtensions);
    return true;
}

bool chooseSwapchainSurfaceFormat(
    uint32_t availableFormatCount,
    VkSurfaceFormatKHR *availableFormats,
    VkSurfaceFormatKHR *chosenFormat)
{
    for (uint32_t i = 0; i < availableFormatCount; i++)
        if (
            availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            *chosenFormat = availableFormats[i];
            return true;
        }
    return false;
}

bool chooseSwapchainPresentMode(
    uint32_t availablePresentModeCount,
    VkPresentModeKHR *availablePresentModes,
    VkPresentModeKHR *chosenPresentMode)
{
    for (uint32_t i = 0; i < availablePresentModeCount; i++)
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            *chosenPresentMode = availablePresentModes[i];
            return true;
        }
    for (uint32_t i = 0; i < availablePresentModeCount; i++)
        if (availablePresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
        {
            *chosenPresentMode = availablePresentModes[i];
            return true;
        }
    return false;
}

// return true if physical device is suitable
bool getPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, PhysicalDeviceProperties *physicalDeviceProperties)
{
    // QUEUE FAMILIES

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    bool foundGraphicsFamily = false;
    bool foundPresentFamily = false;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            physicalDeviceProperties->graphicsFamilyIndex = i;
            foundGraphicsFamily = true;
        }
        VkBool32 presentSupport;
        handleVkResult(
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport),
            "checking queue family surface support to see if its suitable for a present family");
        if (presentSupport)
        {
            physicalDeviceProperties->presentFamilyIndex = i;
            foundPresentFamily = true;
        }
    }

    if (!foundGraphicsFamily || !foundPresentFamily)
        return false;

    free(queueFamilies);

    // EXTENSIONS

    if (!checkDeviceExtensionSupport(physicalDevice))
        return false;

    // SURFACE CAPABILITIES

    handleVkResult(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &physicalDeviceProperties->surfaceCapabilities),
        "getting physical device surface capabilities");

    // SURFACE FORMATS

    uint32_t surfaceFormatCount;
    handleVkResult(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL),
        "getting physical device surface formats count");
    VkSurfaceFormatKHR *surfaceFormats =
        (VkSurfaceFormatKHR *)malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
    handleVkResult(
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats),
        "getting physical device surface formats");

    if (!chooseSwapchainSurfaceFormat(surfaceFormatCount, surfaceFormats, &physicalDeviceProperties->surfaceFormat))
        return false;

    // PRESENT MODES

    uint32_t presentModeCount;
    handleVkResult(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL),
        "getting surface present modes count for swapchain creation");
    VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    handleVkResult(
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes),
        "getting surface present modes for swapchain creation");

    if (!chooseSwapchainPresentMode(presentModeCount, presentModes, &physicalDeviceProperties->presentMode))
        return false;

    return true;
}

void sellectPhysicalDevice(
    VkInstance instance,
    VkSurfaceKHR surface,
    VkPhysicalDevice *physicalDevice,
    PhysicalDeviceProperties *physicalDeviceProperties)
{
    uint32_t physicalDeviceCount;
    handleVkResult(
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL),
        "getting physical device count");
    if (physicalDeviceCount == 0)
    {
        puts("Exiting because could not find GPU with vulkan support");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice *physicalDevices = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
    handleVkResult(
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices),
        "enumerating physical devices");

    for (uint32_t i = 0; i < physicalDeviceCount; i++)
    {
        if (getPhysicalDeviceProperties(physicalDevices[i], surface, physicalDeviceProperties))
        {
            *physicalDevice = physicalDevices[i];
            free(physicalDevices);
            return;
        }
    }
    free(physicalDevices);

    puts("Exiting because could not find suitable GPU");
    exit(EXIT_FAILURE);
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, PhysicalDeviceProperties physicalDeviceProperties)
{
    uint32_t queueFamilyCount;
    uint32_t queueFamilyIndices[2];

    if (physicalDeviceProperties.graphicsFamilyIndex == physicalDeviceProperties.presentFamilyIndex)
    {
        queueFamilyCount = 1;
        queueFamilyIndices[0] = physicalDeviceProperties.graphicsFamilyIndex;
    }
    else
    {
        queueFamilyCount = 2;
        queueFamilyIndices[0] = physicalDeviceProperties.graphicsFamilyIndex;
        queueFamilyIndices[1] = physicalDeviceProperties.presentFamilyIndex;
    }

    VkDeviceQueueCreateInfo *queueCreateInfos = (VkDeviceQueueCreateInfo *)malloc(sizeof(VkDeviceQueueCreateInfo) * queueFamilyCount);

    float generalQueuePriority = 1.0;
    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].pNext = NULL;
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].queueFamilyIndex = queueFamilyIndices[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].pQueuePriorities = &generalQueuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures;
    memset(&deviceFeatures, 0, sizeof(deviceFeatures));

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = queueFamilyCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    if (validationLayersEnabled)
    {
        deviceCreateInfo.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        deviceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS;
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = NULL;
    }
    deviceCreateInfo.enabledExtensionCount = sizeof(REQUIRED_DEVICE_EXTENSIONS) / sizeof(REQUIRED_DEVICE_EXTENSIONS[0]);
    deviceCreateInfo.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    VkDevice device;
    handleVkResult(
        vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device),
        "creating logical device");
    return device;
}
