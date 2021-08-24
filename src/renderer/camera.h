#ifndef CAMERA
#define CAMERA

#include <cglm/types.h>

typedef struct {
    vec3 pos;
    float yaw;
    float pitch;
    float aspectRatio;
    float fov;
    float nearClip;
    float farClip;
} Camera;

void getCameraForward(Camera camera, vec3 forward);
void getCameraRight(Camera camera, vec3 right);

void createViewMat(Camera camera, mat4 viewMat);
void createProjMat(Camera camera, mat4 projMat);

#endif
