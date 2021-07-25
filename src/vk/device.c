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

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, QueueFamilies *queueFamilies)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);

    *queueFamilies = findQueueFamilies(physicalDevice, surface);
    if (queueFamilies->graphicsFamily == QUEUE_FAMILY_DOES_NOT_EXIST ||
        queueFamilies->presentFamily == QUEUE_FAMILY_DOES_NOT_EXIST)
    {
        return false;
    }

    return true;
}

void sellectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice *physicalDevice, QueueFamilies *queueFamilies)
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
        if (isDeviceSuitable(physicalDevices[i], surface, queueFamilies))
        {
            *physicalDevice = physicalDevices[0];
            free(physicalDevices);
            return;
        }
    }

    puts("Exiting because could not find suitable GPU");
    exit(EXIT_FAILURE);
}

QueueFamilies findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
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

    QueueFamilies importantQueueFamilies;
    importantQueueFamilies.graphicsFamily = QUEUE_FAMILY_DOES_NOT_EXIST;
    importantQueueFamilies.presentFamily = QUEUE_FAMILY_DOES_NOT_EXIST;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            importantQueueFamilies.graphicsFamily = i;
        VkBool32 presentSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport)
            importantQueueFamilies.presentFamily = i;
    }
    free(queueFamilies);
    return importantQueueFamilies;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilies queueFamilies)
{
    uint32_t queueFamilyCount;
    uint32_t queueFamilyIndices[2];

    if (queueFamilies.graphicsFamily == queueFamilies.presentFamily)
    {
        queueFamilyCount = 1;
        queueFamilyIndices[0] = queueFamilies.graphicsFamily;
    }
    else
    {
        queueFamilyCount = 2;
        queueFamilyIndices[0] = queueFamilies.graphicsFamily;
        queueFamilyIndices[1] = queueFamilies.presentFamily;
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
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    VkDevice device;
    handleVkResult(
        vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device),
        "creating logical device");
    return device;
}
