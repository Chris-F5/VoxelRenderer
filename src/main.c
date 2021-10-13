#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "object.h"
#include "pointmap_object_loader.h"
#include "renderer/renderer.h"
#include "renderer/scene_data/debug_line.h"
#include "renderer/scene_data/scene_data.h"
#include "renderer/vk_utils/buffer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const float moveSpeed = 0.5f;
const float rotSpeed = 2.0f;

void glfwErrorCallback(int _, const char* errorString)
{
    printf("Exiting because of GLFW error: '%s'\n", errorString);
    exit(EXIT_FAILURE);
}

void write24CubeDebugLinePoints(DebugLineVertex* points, vec3 pos, vec3 size, vec3 color)
{
    points[0] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[1] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };

    points[2] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[3] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };

    points[4] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[5] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };

    points[6] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[7] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };

    points[8] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
    points[9] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };

    points[10] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
    points[11] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };

    points[12] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
    points[13] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };

    points[14] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
    points[15] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };


    points[16] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[17] = (DebugLineVertex) {
        { pos[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
    
    points[18] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[19] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };

    points[20] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[21] = (DebugLineVertex) {
        { pos[0] + size[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };

    points[22] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] },
        { color[0], color[1], color[2] }
    };
    points[23] = (DebugLineVertex) {
        { pos[0], pos[1] + size[1], pos[2] + size[2] },
        { color[0], color[1], color[2] }
    };
}

void updateDebugLinesForObject(VkDevice device, VkPhysicalDevice physicalDevice, Object* object, DebugLineData* debugLineData)
{
    uint32_t pointCount = 24;
    DebugLineVertex* points = (DebugLineVertex*)
        malloc(pointCount * sizeof(DebugLineVertex));

    write24CubeDebugLinePoints(
        points,
        object->pos,
        (vec3){object->width * VOX_BLOCK_SCALE, object->height * VOX_BLOCK_SCALE, object->depth * VOX_BLOCK_SCALE},
        (vec3){1.0f, 0.0f, 0.0f});

   updateDebugLineData(
       device,
       physicalDevice,
       pointCount,
       points,
       debugLineData);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", NULL, NULL);

    Renderer renderer = createRenderer(window);

    FILE* pointFile;

    pointFile = fopen("monu1.ply", "rb");

    Object objectA = loadPointmapObjectFile(renderer.device, renderer.physicalDevice, &renderer.sceneData, (vec3) { 0, 0, 0 }, pointFile);

    fclose(pointFile);

    Object objectB = createEmptyObject(
        &renderer.sceneData,
        (vec3) { 0, -8, 0 },
        1,
        1,
        1,
        objectA.palette);

    setObjectVoxel(
        renderer.device,
        renderer.physicalDevice,
        &renderer.sceneData,
        &objectB,
        (ivec3) { 1, 0, 0 },
        2);

    updateObjectVertexBuffers(
        renderer.device,
        &renderer.sceneData,
        objectB);

    updateDebugLinesForObject(
        renderer.device,
        renderer.physicalDevice,
        &objectA,
        &renderer.sceneData.debugLineData);

    recreateCommandBuffers(&renderer);

    glm_vec3_copy((vec3) { 85.0f, 85.0f, 85.0f }, renderer.camera.pos);
    renderer.camera.yaw = 225;
    renderer.camera.pitch = -20;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            renderer.camera.yaw -= rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            renderer.camera.yaw += rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            renderer.camera.pitch += rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            renderer.camera.pitch -= rotSpeed;
        if (renderer.camera.pitch > 89.9f)
            renderer.camera.pitch = 89.9f;
        if (renderer.camera.pitch < -89.9f)
            renderer.camera.pitch = -89.9f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            vec3 forward;
            getCameraForward(renderer.camera, forward);
            forward[1] = 0;
            glm_normalize(forward);
            glm_vec3_scale(forward, moveSpeed, forward);
            glm_vec3_add(renderer.camera.pos, forward, renderer.camera.pos);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            vec3 forward;
            getCameraForward(renderer.camera, forward);
            forward[1] = 0;
            glm_normalize(forward);
            glm_vec3_scale(forward, -moveSpeed, forward);
            glm_vec3_add(renderer.camera.pos, forward, renderer.camera.pos);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            vec3 right;
            getCameraRight(renderer.camera, right);
            glm_vec3_scale(right, moveSpeed, right);
            glm_vec3_add(renderer.camera.pos, right, renderer.camera.pos);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            vec3 right;
            getCameraRight(renderer.camera, right);
            glm_vec3_scale(right, -moveSpeed, right);
            glm_vec3_add(renderer.camera.pos, right, renderer.camera.pos);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            renderer.camera.pos[1] += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            renderer.camera.pos[1] -= moveSpeed;

        drawFrame(&renderer);
    }

    cleanupObject(&renderer.sceneData, objectA);
    cleanupObject(&renderer.sceneData, objectB);
    cleanupRenderer(renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
