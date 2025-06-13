#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outRoughnessMetallic;

layout(set = 1, binding = 0) uniform BasicMatUBO {
    vec4 baseColor;
} material;

void main() {
    outAlbedo = material.baseColor;
    outNormal = vec4(normalize(fragNormal), 0.0);
    outRoughnessMetallic = vec4(0.0, 0.0, 0.0, 0.0);
}



// #version 450
// layout(location = 0) out vec4 outAlbedo;
// layout(location = 1) out vec4 outNormal;
// layout(location = 2) out vec4 outRoughMetal;
//
// void main() {
//     outAlbedo = vec4(1.0, 1.0, .0, 1.0); // yellow
//     outNormal = vec4(.0, 1.0, 1.0, 1.0); // cyan
//     outRoughMetal= vec4(.0, 1.0, .0, 1.0); // green
// }
