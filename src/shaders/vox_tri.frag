#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

vec3 lightingVector = {-1, -1, -0.5};
float ambient = 0.4;

void main() {
    outColor = vec4(fragColor, 1.0);
}
