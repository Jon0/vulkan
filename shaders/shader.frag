#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

vec4 pointLight = vec4(12.0f, 4.0f, 12.0f, 1.0f);


void main() {
    vec3 toLight = vec3(pointLight - fragPos);
    float light = dot(fragNormal, toLight) / length(toLight);
    light = clamp(light, 0.0, 1.0);
    outColor = vec4(fragColor, 1.0) * light;
}
