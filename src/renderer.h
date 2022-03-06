#ifndef RENDERER_H
#define RENDERER_H

#define MAX_FRAMES_IN_FLIGHT 2

#include <vulkan/vulkan.h>

#include <cglm/types.h>

#include "./debug_lines.h"
#include "./models.h"
#include "./vulkan_device.h"

typedef struct {
    mat4 view;
    mat4 proj;
} CameraRenderData;

typedef struct {
    /* IMAGES */
    VkExtent2D presentExtent;

    VkFormat swapImageFormat;
    uint32_t swapLen;
    VkSwapchainKHR swapchain;
    VkImage* swapImages;
    VkImageView* swapImageViews;

    VkFormat depthImageFormat;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    /* CAMERA DESCRIPTORS */
    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorPool cameraDescriptorPool;
    VkDescriptorSet* cameraDescriptorSets;

    VkBuffer cameraBuffer;
    VkDeviceMemory cameraBufferMemory;

    /* SCENE DATA */
    ModelStorage modelStorage;
    DebugLineStorage debugLineStorage;

    /* SHADER MODULES */
    VkShaderModule voxTriVertShader;
    VkShaderModule voxTriFragShader;
    VkShaderModule debugLineVertShader;
    VkShaderModule debugLineFragShader;

    /* PIPELINES */
    VkRenderPass renderPass;

    VkPipelineLayout voxTriPipelineLayout;
    VkPipeline voxTriPipeline;

    VkPipelineLayout debugLinePipelineLayout;
    VkPipeline debugLinePipeline;

    /* FRAMEBUFFERS */
    VkFramebuffer* framebuffers;

    /* COMMAND BUFFERS */
    VkCommandBuffer* commandBuffers;

    /* SYNCHRONIZATION */
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence renderFinishedFences[MAX_FRAMES_IN_FLIGHT];

    VkFence** swapchainImageFences;
    int currentFrame;
} Renderer;

void Renderer_init(
    Renderer* renderer,
    VulkanDevice* device,
    VkExtent2D presentExtent);

void Renderer_recreateCommandBuffers(
    Renderer* renderer,
    VulkanDevice* device);

void Renderer_drawFrame(
    Renderer* renderer,
    VulkanDevice* device,
    CameraRenderData cameraData);

void Renderer_destroy(
    Renderer* renderer,
    VkDevice logicalDevice);

#endif
