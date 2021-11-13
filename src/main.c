#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "./renderer.h"
#include "./camera.h"
#include "./models.h"
#include "./pointmap_object_loader.h"
#include "./utils.h"
#include "./vert_gen.h"
#include "./vox_blocks.h"
#include "./vox_object.h"
#include "./vulkan_device.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void glfwErrorCallback(int _, const char* errorString)
{
    printf("Exiting because of GLFW error: '%s'\n", errorString);
    exit(EXIT_FAILURE);
}

static VkExtent2D choosePresentExtent(
    GLFWwindow* window,
    VkSurfaceCapabilitiesKHR surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        return surfaceCapabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D extent;

        extent.width = min(
            surfaceCapabilities.maxImageExtent.width,
            max(
                width,
                surfaceCapabilities.maxImageExtent.width));
        extent.height = min(
            surfaceCapabilities.maxImageExtent.height,
            max(
                height,
                surfaceCapabilities.maxImageExtent.height));

        return extent;
    }
}

int main()
{
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", NULL, NULL);

    VulkanDevice device;
    VulkanDevice_init(&device, window);

    VkExtent2D extent = choosePresentExtent(window, device.physicalProperties.surfaceCapabilities);

    Renderer renderer;
    Renderer_init(&renderer, &device, extent);

    /* VOXELS */
    VoxBlockStorage blockStorage;
    VoxPaletteStorage paletteStorage;
    {
        VoxBlockStorage_init(&blockStorage);
        VoxPaletteStorage_init(&paletteStorage);

        FILE* pointmapFile = fopen("monu1.ply", "r");

        VoxObject object;
        loadVoxObjectFromPointmapFile(
            device.logical,
            &blockStorage,
            &paletteStorage,
            &renderer.modelStorage,
            (vec3) { 0.0f, 0.0f, 0.0f },
            pointmapFile,
            &object);

        for (int b = 0; b < object.width * object.height * object.depth; b++)
            if (object.blockMask[b]) {
                int x = b % object.width;
                int y = b / object.width % object.height;
                int z = b / (object.width * object.height);
                updateVoxBlockModel(
                    device.logical,
                    &blockStorage,
                    &paletteStorage,
                    &renderer.modelStorage,
                    object.blocks[b],
                    object.palette,
                    object.models[b],
                    (vec3) {
                        (float)(x * VOX_BLOCK_SCALE),
                        (float)(y * VOX_BLOCK_SCALE),
                        (float)(z * VOX_BLOCK_SCALE) });
            }

        Renderer_recreateCommandBuffers(&renderer, &device);
    }

    /* CAMERA */
    Camera camera;
    {
        float aspectRatio = (float)renderer.presentExtent.width / (float)renderer.presentExtent.height;
        Camera_init(&camera, aspectRatio);
        camera.pos[0] = 100.0f;
        camera.pos[1] = 100.0f;
        camera.pos[2] = 100.0f;
        camera.yaw = 225;
        camera.pitch = -20;
    }

    /* MAIN LOOP */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Camera_userInput(&camera, window);

        CameraRenderData camData;
        Camera_viewMat(&camera, camData.view);
        Camera_projMat(&camera, camData.proj);
        Renderer_drawFrame(&renderer, &device, camData);
    }
    vkDeviceWaitIdle(device.logical);

    VoxBlockStorage_destroy(&blockStorage);
    VoxPaletteStorage_destroy(&paletteStorage);
    Renderer_destroy(&renderer, device.logical);
}
