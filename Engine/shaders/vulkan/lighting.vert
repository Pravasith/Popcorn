#version 450

layout(location = 0) in vec2 inPos; // fullscreen quad positions [-1..1]

layout(location = 0) out vec2 uv;

void main() {
    uv = inPos * 0.5 + 0.5; // convert from [-1,1] to [0,1]
    gl_Position = vec4(inPos, 0.0, 1.0);
}

