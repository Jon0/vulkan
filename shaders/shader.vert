#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragColor;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * inPosition;
    fragPos = ubo.model * inPosition;
    fragNormal = mat3(ubo.model) * inNormal;
    fragColor = vec3(0.9f, 0.1f, 0.1f);
}
