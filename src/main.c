#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "renderer.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan App", NULL, NULL);

    Renderer renderer = createRenderer(window);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        drawFrame(&renderer);
    }

    cleanupRenderer(renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
