#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "./renderer.h"

#include "./models.h"
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
        VoxPaletteRef palette = VoxPaletteStorage_add(&paletteStorage);

        vec3* paletteColorData;
        paletteColorData = VoxPaletteStorage_getColorData(&paletteStorage, palette);
        paletteColorData[1][1] = 1.0f;

        VoxObject object;
        VoxObject_init(
            &object,
            &blockStorage,
            (vec3) { 0.0f, 0.0f, 0.0f },
            1,
            1,
            1,
            palette);

        VoxObject_setVoxel(
            &object,
            &blockStorage,
            (ivec3) { 0, 0, 0 },
            1);

        VoxBlockRef block;
        if (!VoxObject_getBlock(
                &object,
                0, 0, 0,
                &block)) {
            puts("block doesd not exist. exiting");
            exit(EXIT_FAILURE);
        }

        ModelRef model = ModelStorage_add(
            &renderer.modelStorage,
            device.logical,
            1000);

        unsigned char* blockColorData;
        blockColorData = VoxBlockStorage_getColorData(&blockStorage, block);

        generateVoxBlockVertices(
            device.logical,
            paletteColorData,
            blockColorData,
            &renderer.modelStorage,
            model);
        ModelUniformData uniformData;
        glm_mat4_identity(uniformData.model);
        ModelStorage_updateUniformData(
            &renderer.modelStorage,
            device.logical,
            model,
            uniformData);

        Renderer_recreateCommandBuffers(&renderer, &device);
    }

    /* CAMERA */
    vec3 camPos;
    CameraRenderData cameraData;
    {
        float aspectRatio = (float)renderer.presentExtent.width / (float)renderer.presentExtent.height;
        camPos[0] = 5.0f;
        camPos[1] = 5.0f;
        camPos[2] = 5.0f;

        glm_lookat(
            camPos,
            (vec3) { 0.0f, 0.0f, 0.0f }, /* look at */
            (vec3) { 0.0f, 1.0f, 0.0f }, /* up vector */
            cameraData.view);
        glm_perspective(
            glm_rad(90.0f), /* fov */
            aspectRatio,
            0.1, /* near clip */
            100, /* far clip */
            cameraData.proj);
        cameraData.proj[1][1] *= -1;
    }

    /* MAIN LOOP */
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        Renderer_drawFrame(&renderer, &device, cameraData);
    }
    vkDeviceWaitIdle(device.logical);

    VoxBlockStorage_destroy(&blockStorage);
    VoxPaletteStorage_destroy(&paletteStorage);
    Renderer_destroy(&renderer, device.logical);
}
