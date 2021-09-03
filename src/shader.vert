#version 450

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    mat4 view;
    mat4 proj;
} global;

layout(set = 1, binding = 0) uniform MeshUniformBuferObject {
    mat4 model;
} mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = global.proj * global.view * mesh.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
