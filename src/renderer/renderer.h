#ifndef RENDERER
#define RENDERER

#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cglm/types.h>

#include "camera.h"
#include "descriptor_set.h"
#include "device.h"
#include "graphics_pipeline.h"
#include "swapchain.h"

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
    VkFramebuffer* framebuffers;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer indexStagingBuffer;
    VkDeviceMemory indexStagingBufferMemory;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet* descriptorSets;
    VkBuffer* uniformBuffers;
    VkDeviceMemory* uniformBuffersMemory;
    VkCommandPool graphicsCommandPool;
    VkCommandPool transientGraphicsCommandPool;
    VkCommandBuffer* commandBuffers;
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence renderFinishedFences[MAX_FRAMES_IN_FLIGHT];
    int* swapchainImagesInFlight;
    int currentFrame;
    Camera camera;
} Renderer;

Renderer createRenderer(GLFWwindow* window);
void drawFrame(Renderer* r);
void cleanupRenderer(Renderer r);

#endif
