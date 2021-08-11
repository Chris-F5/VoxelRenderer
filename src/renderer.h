#ifndef RENDERER
#define RENDERER

#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer/device.h"
#include "renderer/swapchain.h"
#include "renderer/graphics_pipeline.h"

#define MAX_FRAMES_IN_FLIGHT 2

#define SWAPCHAIN_IMAGE_NOT_IN_FLIGHT -1

typedef struct
{
    VkInstance instance;
    bool validationLayersEnabled;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    PhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    Swapchain swapchain;
    GraphicsPipeline graphicsPipeline;
    VkFramebuffer *framebuffers;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkCommandPool graphicsCommandPool;
    VkCommandBuffer *commandBuffers;
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence renderFinishedFences[MAX_FRAMES_IN_FLIGHT];
    int *swapchainImagesInFlight;
    int currentFrame;
} Renderer;

Renderer createRenderer(GLFWwindow *window);
void drawFrame(Renderer *r);
void cleanupRenderer(Renderer r);

#endif