#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

vec3 lightingVector = {-1, -1, -0.5};
float ambient = 0.4;

void main() {
    //float normalLight = (dot(fragNormal, normalize(lightingVector)) - 1) / -2;
    float normalLight = max(dot(fragNormal, normalize(lightingVector)) * -1, 0);

    float brightness = normalLight * (1 - ambient) + ambient;

    outColor = vec4(fragColor * brightness, 1.0);
}
