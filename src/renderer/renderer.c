#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

#include "depth_buffer.h"
#include "global_descriptor_set.h"
#include "render_command_buffer.h"
#include "shader_module.h"

#include "vk_utils/buffer.h"
#include "vk_utils/command_buffer.h"
#include "vk_utils/exceptions.h"

Renderer createRenderer(GLFWwindow* window)
{
    Renderer r;

    // VALIDATION LAYER SUPPORT

#ifdef DEBUG
    r.validationLayersEnabled = checkValidationLayerSupport();
    if (r.validationLayersEnabled)
        puts("Validation layers enabled");
    else
        puts("Debug mode on but validation layers are not supported");
#else
    r.validationLayersEnabled = false;
#endif

    // DEVICE AND SURFACE

    r.instance = createInstance("Vulkan App", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0, r.validationLayersEnabled);

    handleVkResult(
        glfwCreateWindowSurface(r.instance, window, NULL, &r.surface),
        "creating surface");

    selectPhysicalDevice(r.instance, r.surface, &r.physicalDevice, &r.physicalDeviceProperties);

    r.device = createLogicalDevice(
        r.physicalDevice,
        r.physicalDeviceProperties,
        r.validationLayersEnabled);

    vkGetDeviceQueue(r.device, r.physicalDeviceProperties.graphicsFamilyIndex, 0, &r.graphicsQueue);

    vkGetDeviceQueue(r.device, r.physicalDeviceProperties.presentFamilyIndex, 0, &r.presentQueue);

    r.swapchain = createSwapchain(r.device, r.physicalDevice, r.physicalDeviceProperties, window, r.surface);

    // DEPTH BUFFER

    createDepthBuffer(
        r.device,
        r.physicalDevice,
        r.physicalDeviceProperties.depthBufferFormat,
        r.swapchain.extent,
        &r.depthImage,
        &r.depthImageMemory,
        &r.depthImageView);

    // COMMAND POOLS

    r.graphicsCommandPool = createCommandPool(r.device, 0, r.physicalDeviceProperties.graphicsFamilyIndex);
    r.transientGraphicsCommandPool = createCommandPool(
        r.device,
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        r.physicalDeviceProperties.graphicsFamilyIndex);

    // GPU MEMORY AND DESCRIPTOR SETS

    r.globalDescriptorSetLayout = createGlobalDescriptorSetLayout(r.device);
    r.globalDescriptorSets = malloc(r.swapchain.imageCount * sizeof(VkDescriptorSet));
    r.globalUniformBuffers = malloc(r.swapchain.imageCount * sizeof(VkBuffer));
    r.globalUniformBuffersMemory = malloc(r.swapchain.imageCount * sizeof(VkDeviceMemory));
    createGlobalDescriptorSets(
        r.device,
        r.physicalDevice,
        r.globalDescriptorSetLayout,
        r.swapchain.imageCount,
        &r.globalDescriptorPool,
        r.globalDescriptorSets,
        r.globalUniformBuffers,
        r.globalUniformBuffersMemory);

    // SCEND DATA

    r.sceneData = createSceneData(
        r.device,
        r.physicalDevice,
        1024,
        12);

    // GRAPHICS PIPELINE

    VkShaderModule vertShader = createShaderModule(r.device, "shader.vert.spv");
    VkShaderModule fragShader = createShaderModule(r.device, "shader.frag.spv");

    VkDescriptorSetLayout descriptorSetLayouts[] = { r.globalDescriptorSetLayout, r.sceneData.blocksDescriptorSetLayout };
    r.graphicsPipeline = createGraphicsPipeline(
        r.device,
        r.physicalDeviceProperties,
        r.swapchain,
        vertShader,
        fragShader,
        sizeof(descriptorSetLayouts) / sizeof(descriptorSetLayouts[0]),
        descriptorSetLayouts);

    vkDestroyShaderModule(r.device, vertShader, NULL);
    vkDestroyShaderModule(r.device, fragShader, NULL);

    // FRAMEBUFFERS

    r.framebuffers = (VkFramebuffer*)malloc(r.swapchain.imageCount * sizeof(VkFramebuffer));
    for (int i = 0; i < r.swapchain.imageCount; i++) {
        VkImageView attachments[] = { r.swapchain.imageViews[i], r.depthImageView };

        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = NULL;
        framebufferCreateInfo.flags = 0;
        framebufferCreateInfo.renderPass = r.graphicsPipeline.renderPass;
        framebufferCreateInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = r.swapchain.extent.width;
        framebufferCreateInfo.height = r.swapchain.extent.height;
        framebufferCreateInfo.layers = 1;

        handleVkResult(
            vkCreateFramebuffer(r.device, &framebufferCreateInfo, NULL, &r.framebuffers[i]),
            "creating framebuffer");
    }

    // COMMAND BUFFERS

    r.commandBuffers = (VkCommandBuffer*)malloc(r.swapchain.imageCount * sizeof(VkCommandBuffer));
    createRenderCommandBuffers(
        r.device,
        r.graphicsCommandPool,
        r.swapchain.imageCount,
        r.graphicsPipeline.renderPass,
        r.swapchain.extent,
        r.graphicsPipeline.pipeline,
        r.graphicsPipeline.pipelineLayout,
        r.framebuffers,
        r.globalDescriptorSets,
        &r.sceneData,
        r.commandBuffers);

    // SYNCHRONIZATION OBJECTS

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = NULL;
    semaphoreCreateInfo.flags = 0;

    VkFenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = NULL;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        handleVkResult(
            vkCreateSemaphore(r.device, &semaphoreCreateInfo, NULL, &r.imageAvailableSemaphores[i]),
            "creating image available semaphore");
        handleVkResult(
            vkCreateSemaphore(r.device, &semaphoreCreateInfo, NULL, &r.renderFinishedSemaphores[i]),
            "creating render finished semaphore");
        handleVkResult(
            vkCreateFence(r.device, &fenceCreateInfo, NULL, &r.renderFinishedFences[i]),
            "creating render finished fence");
    }

    r.swapchainImagesInFlight = (int*)malloc(r.swapchain.imageCount * sizeof(int));
    for (int i = 0; i < r.swapchain.imageCount; i++)
        r.swapchainImagesInFlight[i] = SWAPCHAIN_IMAGE_NOT_IN_FLIGHT;

    r.currentFrame = 0;

    glm_vec3_zero(r.camera.pos);
    r.camera.yaw = 0.0f;
    r.camera.pitch = 0.0f;
    r.camera.aspectRatio = (float)r.swapchain.extent.width / (float)r.swapchain.extent.height;
    r.camera.fov = 90.0f;
    r.camera.nearClip = 0.1f;
    r.camera.farClip = 1000.0f;

    return r;
}

void recreateCommandBuffers(Renderer* r)
{
    vkDeviceWaitIdle(r->device);
    createRenderCommandBuffers(
        r->device,
        r->graphicsCommandPool,
        r->swapchain.imageCount,
        r->graphicsPipeline.renderPass,
        r->swapchain.extent,
        r->graphicsPipeline.pipeline,
        r->graphicsPipeline.pipelineLayout,
        r->framebuffers,
        r->globalDescriptorSets,
        &r->sceneData,
        r->commandBuffers);
}

void drawFrame(Renderer* r)
{
    vkWaitForFences(r->device, 1, &r->renderFinishedFences[r->currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    handleVkResult(
        vkAcquireNextImageKHR(r->device, r->swapchain.swapchain, UINT64_MAX, r->imageAvailableSemaphores[r->currentFrame], VK_NULL_HANDLE, &imageIndex),
        "acquiring next swapchain image");

    if (r->swapchainImagesInFlight[imageIndex] != SWAPCHAIN_IMAGE_NOT_IN_FLIGHT)
        vkWaitForFences(r->device, 1, &r->renderFinishedFences[r->swapchainImagesInFlight[imageIndex]], VK_TRUE, UINT32_MAX);

    vkResetFences(r->device, 1, &r->renderFinishedFences[r->currentFrame]);
    for (int i = 0; i < r->swapchain.imageCount; i++)
        if (r->swapchainImagesInFlight[i] == r->currentFrame) {
            r->swapchainImagesInFlight[i] = SWAPCHAIN_IMAGE_NOT_IN_FLIGHT;
            break;
        }
    r->swapchainImagesInFlight[imageIndex] = r->currentFrame;

    GlobalUniformBuffer globalUniformData;
    createViewMat(r->camera, globalUniformData.view);
    createProjMat(r->camera, globalUniformData.proj);
    copyDataToBuffer(
        r->device,
        &globalUniformData,
        r->globalUniformBuffersMemory[imageIndex],
        0,
        sizeof(GlobalUniformBuffer));

    //glm_mat4_identity(r->blockA.descriptorData.model);
    //glm_rotate(r->blockA.descriptorData.model, (float)glfwGetTime() * 0.8f, (vec3) { 0.0f, 0.0f, 1.0f });
    //glm_translate(r->blockA.descriptorData.model, (vec3) { 0.0f, 0.0f, 3.0f });
    //updateBlockDescriptors(r->device, &r->blockA, imageIndex);

    //glm_mat4_identity(r->blockB.descriptorData.model);
    //updateBlockDescriptors(r->device, &r->blockB, imageIndex);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &r->imageAvailableSemaphores[r->currentFrame];
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &r->commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &r->renderFinishedSemaphores[r->currentFrame];

    handleVkResult(
        vkQueueSubmit(r->graphicsQueue, 1, &submitInfo, r->renderFinishedFences[r->currentFrame]),
        "submitting render command buffer");

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &r->renderFinishedSemaphores[r->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &r->swapchain.swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;

    handleVkResult(
        vkQueuePresentKHR(r->presentQueue, &presentInfo),
        "submitting present call");

    r->currentFrame = (r->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void cleanupRenderer(Renderer r)
{
    vkDeviceWaitIdle(r.device);

    free(r.commandBuffers);
    free(r.swapchainImagesInFlight);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(r.device, r.imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(r.device, r.renderFinishedSemaphores[i], NULL);
        vkDestroyFence(r.device, r.renderFinishedFences[i], NULL);
    }

    vkDestroyDescriptorPool(r.device, r.globalDescriptorPool, NULL);
    free(r.globalDescriptorSets);
    for (int i = 0; i < r.swapchain.imageCount; i++) {
        vkDestroyBuffer(r.device, r.globalUniformBuffers[i], NULL);
        vkFreeMemory(r.device, r.globalUniformBuffersMemory[i], NULL);
    }
    free(r.globalUniformBuffers);
    free(r.globalUniformBuffersMemory);

    cleanupSceneData(r.device, r.sceneData);

    vkDestroyDescriptorSetLayout(r.device, r.globalDescriptorSetLayout, NULL);

    vkDestroyCommandPool(r.device, r.graphicsCommandPool, NULL);
    vkDestroyCommandPool(r.device, r.transientGraphicsCommandPool, NULL);

    for (int i = 0; i < r.swapchain.imageCount; i++)
        vkDestroyFramebuffer(r.device, r.framebuffers[i], NULL);
    free(r.framebuffers);

    cleanupGraphicsPipeline(r.device, r.graphicsPipeline);
    cleanupSwapchain(r.device, r.swapchain);

    vkDestroyImage(r.device, r.depthImage, NULL);
    vkFreeMemory(r.device, r.depthImageMemory, NULL);
    vkDestroyImageView(r.device, r.depthImageView, NULL);

    vkDestroyDevice(r.device, NULL);
    vkDestroySurfaceKHR(r.instance, r.surface, NULL);
    vkDestroyInstance(r.instance, NULL);
}
