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

bool checkValidationLayerSupport(void)
{
    uint32_t availableLayerCount;
    vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);
    VkLayerProperties *availableLayers = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

    const int validationLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
    for (int vi = 0; vi < validationLayerCount; vi++)
    {
        bool layerFound = false;
        for (int li = 0; li < availableLayerCount; li++)
            if (strcmp(VALIDATION_LAYERS[vi], availableLayers[li].layerName) == 0)
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
    handleVkResult(vkCreateInstance(&createInfo, NULL, &instance), "creating vk instance");

    return instance;
}

bool isDeviceSuitable(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

    return true;
}

VkPhysicalDevice sellectPhysicalDevice(VkInstance instance)
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

    VkPhysicalDevice physicalDevice = physicalDevices[0];
    free(physicalDevices);

    return physicalDevice;
}

uint32_t findGraphicsQueueFamily(VkPhysicalDevice physicalDevice)
{
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);

    if (queueFamilyCount == UINT32_MAX)
    {
        puts("Exiting because physical device claims to have UINT32_MAX queue families.");
        exit(EXIT_FAILURE);
    }

    VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    uint32_t graphicsFamilyIndex = QUEUE_FAMILY_DOES_NOT_EXIST;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamilyIndex = i;
        }
    }
    free(queueFamilies);
    return graphicsFamilyIndex;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamilyIndex)
{
    float graphicsQueuePriority = 1.0;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo;
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.pNext = NULL;
    graphicsQueueCreateInfo.flags = 0;
    graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;

    VkPhysicalDeviceFeatures deviceFeatures;
    memset(&deviceFeatures, 0, sizeof(deviceFeatures));

    VkDeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &graphicsQueueCreateInfo;
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
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    VkDevice device;
    handleVkResult(
        vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device),
        "creating logical device");
    return device;
}
