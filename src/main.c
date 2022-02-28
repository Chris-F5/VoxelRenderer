#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "./camera.h"
#include "./chunk_lighting.h"
#include "./chunks.h"
#include "./models.h"
#include "./normal_gen.h"
#include "./pointmap_object_loader.h"
#include "./renderer.h"
#include "./sparse_vox_object_loader.h"
#include "./utils.h"
#include "./vert_gen.h"
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

    VkExtent2D extent = choosePresentExtent(
        window,
        device.physicalProperties.surfaceCapabilities);

    Renderer renderer;
    Renderer_init(&renderer, &device, extent);

    /* PALETTE */
    VoxPaletteStorage paletteStorage;
    VoxPaletteRef chunkPalette;
    VoxPaletteStorage_init(&paletteStorage);
    chunkPalette = VoxPaletteStorage_add(&paletteStorage);

    /* CHUNK STORAGE */
    ChunkStorage chunkStorage;
    ChunkGpuStorage chunkGpuStorage;
    {
        ChunkStorage_init(&chunkStorage);
        ChunkGpuStorage_init(&chunkGpuStorage, device.logical, device.physical);
        FILE* svoFile = fopen("object1.svo", "r");
        loadChunksFromSparseVoxFile(
            device.logical,
            &chunkStorage,
            &chunkGpuStorage,
            &paletteStorage,
            chunkPalette,
            svoFile);
        fclose(svoFile);
    }

    /* FETCH ALL CHUNK IDS */
    ChunkRef* allChunks;
    {
        ChunkRef chunk;
        allChunks = (ChunkRef*)malloc(
            chunkStorage.idAllocator.count * sizeof(ChunkRef));
        uint32_t i = 0;
        if (IdAllocator_first(&chunkStorage.idAllocator, &chunk))
            do {
                if (i >= chunkStorage.idAllocator.count) {
                    puts("chunk id allocator count does not match iterator (<). exiting");
                    exit(EXIT_FAILURE);
                }
                allChunks[i] = chunk;
                i += 1;
            } while (IdAllocator_next(&chunkStorage.idAllocator, chunk, &chunk));
        if (i < chunkStorage.idAllocator.count) {
            puts("chunk id allocator count does not match iterator (>). exiting");
            exit(EXIT_FAILURE);
        }
    }

    /* CHUNK NORMAL GEN */
    NormalGen normalGen;
    {
        NormalGen_init(
            &normalGen,
            &chunkGpuStorage,
            device.logical,
            device.transientGraphicsCommandPool);

        /*
        NormalGen_generateNormals(
            &normalGen,
            device.logical,
            device.graphicsQueue,
            chunkStorage.idAllocator.count,
            allChunks);
        */
    }

    /* CHUNK LIGHTING */
    ChunkLighting chunkLighting;
    {

        ChunkLighting_init(
            &chunkLighting,
            &chunkGpuStorage,
            device.logical,
            device.transientGraphicsCommandPool);

        /* UPDATE CHUNK LIGHTING */
        vec4 lightDir = { -1.0f, -2.0f, -1.5f };
        glm_vec3_normalize(lightDir);

        ChunkLighting_directLightingPass(
            &chunkLighting,
            device.logical,
            device.graphicsQueue,
            chunkStorage.idAllocator.count,
            allChunks,
            lightDir);
        for (int i = 0; i < 100; i++) {
            ChunkLighting_diffuseLightingPass(
                &chunkLighting,
                device.logical,
                device.graphicsQueue,
                chunkStorage.idAllocator.count,
                allChunks,
                lightDir);
        }
    }

    /* GENERATE CHUNK MESHES */
    ModelRef* models = (ModelRef*)malloc(CHUNK_CAPACITY * sizeof(ModelRef));
    ChunkVertGen vertGen;
    {
        ChunkVertGen_init(&vertGen);
        ChunkRef chunk;
        if (IdAllocator_first(&chunkStorage.idAllocator, &chunk)) {
            do {
                ChunkVertGen_generate(
                    &vertGen,
                    &chunkStorage,
                    &chunkGpuStorage,
                    device.logical,
                    chunk,
                    &paletteStorage,
                    chunkPalette);

                models[chunk] = ModelStorage_add(
                    &renderer.modelStorage,
                    device.logical,
                    vertGen.vertCount);

                ModelStorage_updateVertexData(
                    &renderer.modelStorage,
                    device.logical,
                    models[chunk],
                    vertGen.vertCount,
                    vertGen.vertBuffer);

                vec3 worldPos;
                worldPos[0] = chunkStorage.positions[chunk][0] * CHUNK_SCALE;
                worldPos[1] = chunkStorage.positions[chunk][1] * CHUNK_SCALE;
                worldPos[2] = chunkStorage.positions[chunk][2] * CHUNK_SCALE;

                ModelUniformData modelData;
                glm_translate_make(modelData.model, worldPos);

                ModelStorage_updateUniformData(
                    &renderer.modelStorage,
                    device.logical,
                    models[chunk],
                    modelData);
            } while (IdAllocator_next(&chunkStorage.idAllocator, chunk, &chunk));

            Renderer_recreateCommandBuffers(&renderer, &device);
        }
    }

    /* CAMERA */
    Camera camera;
    {
        float aspectRatio
            = (float)renderer.presentExtent.width
            / (float)renderer.presentExtent.height;
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

    free(allChunks);
    vkDeviceWaitIdle(device.logical);

    NormalGen_destroy(
        &normalGen,
        device.logical,
        device.transientGraphicsCommandPool);
    ChunkLighting_destroy(
        &chunkLighting,
        device.logical,
        device.transientGraphicsCommandPool);
    ChunkVertGen_destroy(&vertGen);
    ChunkStorage_destroy(&chunkStorage);
    ChunkGpuStorage_destroy(&chunkGpuStorage, device.logical);
    VoxPaletteStorage_destroy(&paletteStorage);
    Renderer_destroy(&renderer, device.logical);
}
