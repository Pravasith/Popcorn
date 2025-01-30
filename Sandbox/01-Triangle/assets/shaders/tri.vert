#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec2 position = inPos;
    vec3 colors = inColor;

    gl_Position = vec4(position, 0., 1.);
    fragColor = colors;
}
