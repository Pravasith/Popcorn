#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 invViewProj;
} camera;

layout(set = 1, binding = 0) uniform sampler2D lightTex;

void main() {
    outColor = vec4(texture(lightTex, fragUV).rgb, 1.);
    // outColor = vec4(.5, .5, .5, 1.);
}
