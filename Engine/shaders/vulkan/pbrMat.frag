#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outRoughnessMetallic;

layout(set = 1, binding = 0) uniform PbrMaterialUBO {
    vec4 baseColor;

    float metallic;
    float roughness;
    float alphaCutoff;

    int hasBaseColorTex;
    int hasNormalTex;
    int hasMetallicRoughnessTex;

    float pad0;
    float pad1;
} material;

void main() {
    outAlbedo = material.baseColor;
    outNormal = vec4(normalize(fragNormal), 0.0);
    outRoughnessMetallic =
        vec4(material.roughness, material.metallic, 0.0, 0.0);
}
