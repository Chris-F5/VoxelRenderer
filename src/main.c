#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk/device.h"
#include "vk/exceptions.h"
#include "vk/swapchain.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const char *APP_NAME = "Vulkan App";
const uint32_t APP_VERSION = VK_MAKE_VERSION(1, 0, 0);
const uint32_t VULKAN_API_VERSION = VK_API_VERSION_1_0;

bool validationLayersEnabled;

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", NULL, NULL);

#ifdef DEBUG
    validationLayersEnabled = checkValidationLayerSupport();
    if (validationLayersEnabled)
        puts("Validation layers enabled");
    else
        puts("Debug mode on but validation layers are not supported");
#else
    validationLayersEnabled = false;
#endif

    VkInstance instance = createInstance();

    VkSurfaceKHR surface;
    handleVkResult(
        glfwCreateWindowSurface(instance, window, NULL, &surface),
        "creating surface");

    VkPhysicalDevice physicalDevice;
    PhysicalDeviceProperties physicalDeviceProperties;
    sellectPhysicalDevice(instance, surface, &physicalDevice, &physicalDeviceProperties);

    VkDevice device = createLogicalDevice(
        physicalDevice,
        physicalDeviceProperties);

    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, physicalDeviceProperties.graphicsFamilyIndex, 0, &graphicsQueue);
    VkQueue presentQueue;
    vkGetDeviceQueue(device, physicalDeviceProperties.presentFamilyIndex, 0, &presentQueue);

    Swapchain swapchain = createSwapchain(device, physicalDevice, physicalDeviceProperties, window, surface);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    cleanupSwapchain(device, swapchain);

    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
