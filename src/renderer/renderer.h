#ifndef RENDERER
#define RENDERER

#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cglm/types.h>

#include "camera.h"
#include "device.h"
#include "graphics_pipeline.h"
#include "scene_data/scene_data.h"
#include "swapchain.h"

#define MAX_FRAMES_IN_FLIGHT 2

#define SWAPCHAIN_IMAGE_NOT_IN_FLIGHT -1

typedef struct
{
    // DEVICE

    VkInstance instance;
    bool validationLayersEnabled;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    PhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    Swapchain swapchain;

    // MEMORY

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkFramebuffer* framebuffers;

    VkDescriptorSetLayout globalDescriptorSetLayout;
    VkDescriptorPool globalDescriptorPool;
    VkDescriptorSet* globalDescriptorSets;
    VkBuffer* globalUniformBuffers;
    VkDeviceMemory* globalUniformBuffersMemory;

    // PIPELINE

    VkCommandPool transientGraphicsCommandPool;

    VkCommandPool graphicsCommandPool;
    VkCommandBuffer* commandBuffers;

    GraphicsPipeline graphicsPipeline;

    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence renderFinishedFences[MAX_FRAMES_IN_FLIGHT];

    int* swapchainImagesInFlight;
    int currentFrame;

    // SCENE

    SceneData sceneData;

    Camera camera;
} Renderer;

Renderer createRenderer(GLFWwindow* window);

void drawFrame(Renderer* r);

void cleanupRenderer(Renderer r);

#endif
