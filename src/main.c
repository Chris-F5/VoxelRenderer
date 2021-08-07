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
#include "vk/command_buffer.h"

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

    VkCommandPool graphicsCommandPool = createCommandPool(device, 0, physicalDeviceProperties.graphicsFamilyIndex);

    VkCommandBuffer *commandBuffers = (VkCommandBuffer *)malloc(swapchain.imageCount * sizeof(VkCommandBuffer));
    allocateCommandBuffers(device, graphicsCommandPool, swapchain.imageCount, commandBuffers);
    recordRenderCommandBuffers(pipeline.renderPass, swapchain.extent, pipeline.pipeline, swapchain.imageCount, commandBuffers, framebuffers);

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    handleVkResult(
        vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &imageAvailableSemaphore),
        "creating image available semaphore");
    handleVkResult(
        vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &renderFinishedSemaphore),
        "creating render finished semaphore");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        uint32_t imageIndex;
        handleVkResult(
            vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex),
            "acquiring next swapchain image");

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

        handleVkResult(
            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE),
            "submitting render command buffer");

        VkPresentInfoKHR presentInfo;
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = NULL;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain.swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL;

        handleVkResult(
            vkQueuePresentKHR(presentQueue, &presentInfo),
            "submitting present call");

        vkDeviceWaitIdle(device);
    }

    vkDeviceWaitIdle(device);

    vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
    vkDestroySemaphore(device, renderFinishedSemaphore, NULL);

    vkDestroyCommandPool(device, graphicsCommandPool, NULL);

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
