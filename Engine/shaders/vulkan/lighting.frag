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
    // vec3 camPos;
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

// Utility: reconstruct view-space position from depth
vec3 ReconstructViewPosition(vec2 uv, float depth) {
    vec4 clip = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 view = camera.invViewProj * clip;
    return view.xyz / view.w;
}

void main() {
    vec2 rm = texture(roughMetalTex, fragUV).rg;

    vec3 albedo = texture(albedoTex, fragUV).rgb;
    float depth = texture(depthTex, fragUV).r;
    vec3 normal = normalize(texture(normalTex, fragUV).xyz * 2.0 - 1.0);
    float roughness = rm.r;
    float metallic = rm.g;

    vec3 viewPos = ReconstructViewPosition(fragUV, depth);
    vec3 viewNormal = normalize(normal);
    // vec3 viewDir = normalize(camera.camPos - viewPos);
    vec3 viewDir = normalize(vec3(0.0) - viewPos);

    vec3 finalColor = vec3(0.0);

    for (uint i = 0; i < lights.length(); ++i) {
        LightUniform light = lights[i];
        vec3 lightColor = light.color * light.intensity;

        vec3 lightVec;
        float attenuation = 1.0;

        if (light.lightType == 0.0) {
            // Point light
            lightVec = light.position - viewPos;
            float dist = length(lightVec);
            attenuation = 1.0 / (dist * dist);
            lightVec /= dist;
        } else if (light.lightType == 1.0) {
            // Directional light
            lightVec = normalize(-light.direction);
        } else if (light.lightType == 2.0) {
            // // Spotlight
            // lightVec = light.position - viewPos;
            // float dist = length(lightVec);
            // lightVec /= dist;
            // float theta = dot(lightVec, normalize(-light.direction));
            // float epsilon = light.innerConeAngle - light.outerConeAngle;
            // float intensity = clamp((theta - light.outerConeAngle) / epsilon, 0.0, 1.0);
            // attenuation = intensity / (dist * dist);
            // TODO: Later...
            lightVec = normalize(-light.direction);
        }

        float NdotL = max(dot(viewNormal, lightVec), 0.0);
        vec3 diffuse = albedo * lightColor * NdotL;

        finalColor += diffuse * attenuation;
    }

    outColor = vec4(finalColor, 1.0);
}
