#version 450

layout(set = 0, binding = 0) uniform ProjView {
    mat4 view;
    mat4 proj;
} projViewUbo;

layout(set = 1, binding = 0) uniform ModelView {
    mat4 model;
} modelUbo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = projViewUbo.proj * projViewUbo.view * modelUbo.model
        * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}
