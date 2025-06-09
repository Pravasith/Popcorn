#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D lightTex;

void main() {
    // outColor = vec4(texture(lightTex, fragUV).rgb, 1.);
    outColor = vec4(1., 1., 1., 1.);
}
