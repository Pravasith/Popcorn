#version 450

struct LightUniform {
    vec3 position;
    float pad0;
    vec3 direction;
    float pad1;
    vec3 color;
    float pad2;
    float lightType;
    float intensity;
    float innerConeAngle;
    float outerConeAngle;
};

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 invViewProj;
} camera;

layout(set = 1, binding = 0) readonly buffer LightBuffer {
    LightUniform lights[];
};
layout(set = 1, binding = 1) uniform sampler2D albedoTex;
layout(set = 1, binding = 2) uniform sampler2D depthTex;
layout(set = 1, binding = 3) uniform sampler2D normalTex;
layout(set = 1, binding = 4) uniform sampler2D roughMetalTex;

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

vec3 ReconstructWorldSpace(vec2 uv, float depth) {
    // float ndcZ = 1.0 - depth * 2.0;

    float ndcZ = depth * 2.0 - 1.0; // Convert to NDC z
    vec4 clipPos = vec4(uv * 2.0 - 1.0, ndcZ, 1.0);
    vec4 worldPosH = camera.invViewProj * clipPos;
    return worldPosH.xyz / worldPosH.w;
}

void main() {
    vec2 flippedUV = vec2(fragUV.x, 1. - fragUV.y);

    float depth = texture(depthTex, flippedUV).r;
    vec3 albedo = texture(albedoTex, flippedUV).rgb;
    vec3 normal = normalize(texture(normalTex, flippedUV).xyz * 2.0 - 1.0);
    vec2 rm = texture(roughMetalTex, flippedUV).rg;
    float roughness = rm.r;
    float metallic = rm.g;

    vec3 worldPos = ReconstructWorldSpace(flippedUV, depth);

    vec3 finalColor = vec3(0.0);

    for (uint i = 0; i < lights.length(); ++i) {
        LightUniform light = lights[i];
        vec3 lightVec;
        float attenuation = 1.0;

        if (light.lightType == 0.0) {
            // Point light
            lightVec = light.position - worldPos;
            float dist = length(lightVec);
            attenuation = 1.0 / max(dist * dist, 0.01); // Avoid div by zero
            lightVec = normalize(lightVec);
        } else if (light.lightType == 1.0) {
            // Directional light
            lightVec = normalize(-light.direction);
        } else if (light.lightType == 2.0) {
            // Simplified spotlight for now
            lightVec = normalize(-light.direction);
        }

        float NdotL = max(dot(normal, lightVec), 0.0);
        vec3 lightColor = light.color * light.intensity;
        vec3 diffuse = albedo * lightColor * NdotL;

        finalColor += diffuse * attenuation;
    }

    finalColor += albedo * 0.05; // Ambient
    outColor = vec4(finalColor, 1.0);

    // outColor = vec4(vec3(texture(depthTex, fragUV).r), 1.0);
}
