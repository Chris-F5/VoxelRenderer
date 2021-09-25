#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "object.h"
#include "renderer/renderer.h"
#include "renderer/scene_data/scene_data.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const float moveSpeed = 0.0005f;
const float rotSpeed = 0.01f;

void glfwErrorCallback(int _, const char* errorString)
{
    printf("Exiting because of GLFW error: '%s'\n", errorString);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", NULL, NULL);

    Renderer renderer = createRenderer(window);

    FILE* objectFile;

    objectFile = fopen("object.voxobj", "rb");

    Object object = createObject(
        renderer.device,
        renderer.physicalDevice,
        &renderer.sceneData,
        objectFile);

    fclose(objectFile);

    recreateCommandBuffers(&renderer);

    glm_vec3_copy((vec3) { 0.0f, 2.0f, 10.0f }, renderer.camera.pos);
    renderer.camera.yaw = 180;

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

    cleanupObject(&renderer.sceneData, object);
    cleanupRenderer(renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
