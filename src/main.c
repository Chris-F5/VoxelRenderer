#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer/renderer.h"

const uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        drawFrame(&renderer);
    }

    cleanupRenderer(renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
