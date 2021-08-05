#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk/device.h"
#include "vk/exceptions.h"
#include "vk/swapchain.h"
#include "vk/shader_module.h"
#include "vk/graphics_pipeline.h"

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

    VkShaderModule vertShader = createShaderModule(device, "shader.vert.spv");
    VkShaderModule fragShader = createShaderModule(device, "shader.frag.spv");

    GraphicsPipeline pipeline = createGraphicsPipeline(device, swapchain, vertShader, fragShader);

    vkDestroyShaderModule(device, vertShader, NULL);
    vkDestroyShaderModule(device, fragShader, NULL);

    VkFramebuffer *framebuffers = (VkFramebuffer *)malloc(swapchain.imageCount * sizeof(VkFramebuffer));
    for (int i = 0; i < swapchain.imageCount; i++)
    {
        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = NULL;
        framebufferCreateInfo.flags = 0;
        framebufferCreateInfo.renderPass = pipeline.renderPass;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &swapchain.imageViews[i];
        framebufferCreateInfo.width = swapchain.extent.width;
        framebufferCreateInfo.height = swapchain.extent.height;
        framebufferCreateInfo.layers = 1;

        handleVkResult(
            vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, &framebuffers[i]),
            "creating framebuffer");
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    for (int i = 0; i < swapchain.imageCount; i++)
        vkDestroyFramebuffer(device, framebuffers[i], NULL);
    free(framebuffers);

    cleanupGraphicsPipeline(device, pipeline);
    cleanupSwapchain(device, swapchain);

    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
