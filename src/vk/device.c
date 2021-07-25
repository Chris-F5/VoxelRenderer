#include "device.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "exceptions.h"

extern char *APP_NAME;
extern uint32_t APP_VERSION;
extern uint32_t VULKAN_API_VERSION;

const char *VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"};

bool checkValidationLayerSupport()
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

VkInstance createInstance()
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
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
#ifdef DEBUG
    if (checkValidationLayerSupport())
    {
        puts("Validation layers enabled");
        createInfo.enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(VALIDATION_LAYERS[0]);
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS;
    }
    else
    {
        puts("Debug mode on but validation layers are not supported!");
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = NULL;
    }
#else
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
#endif

    VkInstance instance;
    handleVkResult(vkCreateInstance(&createInfo, NULL, &instance), "creating vk instance");

    return instance;
}
