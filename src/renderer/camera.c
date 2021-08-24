#include "camera.h"

#include <cglm/cglm.h>
#include <math.h>

void getCameraForward(Camera camera, vec3 forward)
{
    forward[0] = cos(glm_rad(camera.pitch)) * sin(glm_rad(camera.yaw));
    forward[1] = sin(glm_rad(camera.pitch));
    forward[2] = cos(glm_rad(camera.pitch)) * cos(glm_rad(camera.yaw));
}

void getCameraRight(Camera camera, vec3 right)
{
    vec3 forward;
    getCameraForward(camera, forward);
    vec3 c;
    glm_vec3_copy((vec3) { 0.0f, 1.0f, 0.0f }, c);
    glm_vec3_cross(forward, c, right);
    glm_normalize(right);
}

void createViewMat(Camera camera, mat4 viewMat)
{
    vec3 forward;
    getCameraForward(camera, forward);

    vec3 lookTarget;
    glm_vec3_add(camera.pos, forward, lookTarget);
    glm_lookat(camera.pos, lookTarget, (vec3) { 0.0f, 1.0f, 0.0f }, viewMat);
}

void createProjMat(Camera camera, mat4 projMat)
{
    glm_perspective(glm_rad(camera.fov), camera.aspectRatio, camera.nearClip, camera.farClip, projMat);
    projMat[1][1] *= -1;
}
