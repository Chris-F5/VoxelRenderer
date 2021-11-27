#include "./camera.h"

#include <cglm/cglm.h>

const float moveSpeed = 0.5f;
const float rotSpeed = 2.0f;

static void Camera_forward(const Camera* camera, vec3 forward)
{
    forward[0] = cos(glm_rad(camera->pitch)) * sin(glm_rad(camera->yaw));
    forward[1] = sin(glm_rad(camera->pitch));
    forward[2] = cos(glm_rad(camera->pitch)) * cos(glm_rad(camera->yaw));
}

static void Camera_right(const Camera* camera, vec3 right)
{
    vec3 forward;
    Camera_forward(camera, forward);
    vec3 c;
    glm_vec3_copy((vec3) { 0.0f, 1.0f, 0.0f }, c);
    glm_vec3_cross(forward, c, right);
    glm_normalize(right);
}

void Camera_init(Camera* camera, float aspectRatio)
{
    glm_vec3_zero(camera->pos);
    camera->yaw = 0;
    camera->pitch = 0;
    camera->fov = 90;
    camera->aspectRatio = aspectRatio;
    camera->nearClip = 0.01;
    camera->farClip = 1000;
}

void Camera_userInput(Camera* camera, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera->yaw -= rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera->yaw += rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera->pitch += rotSpeed;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera->pitch -= rotSpeed;
        if (camera->pitch > 89.9f)
            camera->pitch = 89.9f;
        if (camera->pitch < -89.9f)
            camera->pitch = -89.9f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            vec3 forward;
            Camera_forward(camera, forward);
            forward[1] = 0;
            glm_normalize(forward);
            glm_vec3_scale(forward, moveSpeed, forward);
            glm_vec3_add(camera->pos, forward, camera->pos);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            vec3 forward;
            Camera_forward(camera, forward);
            forward[1] = 0;
            glm_normalize(forward);
            glm_vec3_scale(forward, -moveSpeed, forward);
            glm_vec3_add(camera->pos, forward, camera->pos);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            vec3 right;
            Camera_right(camera, right);
            glm_vec3_scale(right, moveSpeed, right);
            glm_vec3_add(camera->pos, right, camera->pos);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            vec3 right;
            Camera_right(camera, right);
            glm_vec3_scale(right, -moveSpeed, right);
            glm_vec3_add(camera->pos, right, camera->pos);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->pos[1] += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->pos[1] -= moveSpeed;
}

void Camera_viewMat(Camera* camera, mat4 view)
{
    vec3 forward;
    Camera_forward(camera, forward);

    vec3 lookTarget;
    glm_vec3_add(camera->pos, forward, lookTarget);
    glm_lookat(camera->pos, lookTarget, (vec3) { 0.0f, 1.0f, 0.0f }, view);
}

void Camera_projMat(Camera* camera, mat4 proj)
{
    glm_perspective(glm_rad(camera->fov), camera->aspectRatio, camera->nearClip, camera->farClip, proj);
    proj[1][1] *= -1;
}