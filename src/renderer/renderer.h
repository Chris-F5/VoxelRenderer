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
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer vertexBufferB;
    VkDeviceMemory vertexBufferMemoryB;
    VkBuffer indexBufferB;
    VkDeviceMemory indexBufferMemoryB;
    VkDescriptorSetLayout globalDescriptorSetLayout;
    VkDescriptorSetLayout meshDescriptorSetLayout;
    VkDescriptorPool globalDescriptorPool;
    VkDescriptorPool meshDescriptorPool;
    VkDescriptorSet* globalDescriptorSets;
    VkBuffer* globalUniformBuffers;
    VkDeviceMemory* globalUniformBuffersMemory;
    VkDescriptorSet* meshDescriptorSets;
    VkBuffer* meshUniformBuffers;
    VkDeviceMemory* meshUniformBuffersMemory;
    VkDescriptorSet* meshBDescriptorSets;
    VkBuffer* meshBUniformBuffers;
    VkDeviceMemory* meshBUniformBuffersMemory;

    // PIPELINE
    VkCommandPool graphicsCommandPool;
    VkCommandPool transientGraphicsCommandPool;
    VkCommandBuffer* commandBuffers;
    GraphicsPipeline graphicsPipeline;
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
