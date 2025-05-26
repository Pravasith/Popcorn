#version 450

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

// G-buffer input textures
layout(set = 0, binding = 1) uniform sampler2D albedoTex;
layout(set = 0, binding = 2) uniform sampler2D depthTex;
layout(set = 0, binding = 3) uniform sampler2D normalTex;
layout(set = 0, binding = 4) uniform sampler2D roughnessMetallicTex;

// Light storage buffer (example structure)
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

layout(set = 0, binding = 0) buffer LightBuffer {
    Light lights[];
} lightBuffer;

// Camera info (usually passed as uniform or push constant)
layout(push_constant) uniform PushConstants {
    mat4 invProjection;
    mat4 invView;
    vec3 cameraPos;
} pc;

vec3 ReconstructPosition(vec2 uv, float depth) {
    // Reconstruct view space position from depth and screen uv
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 viewPos = pc.invProjection * clipPos;
    viewPos /= viewPos.w;
    return viewPos.xyz;
}

void main() {
    vec3 albedo = texture(albedoTex, uv).rgb;
    float depth = texture(depthTex, uv).r;
    vec3 normal = texture(normalTex, uv).xyz * 2.0 - 1.0;
    vec2 roughnessMetallic = texture(roughnessMetallicTex, uv).rg;

    vec3 posView = ReconstructPosition(uv, depth);
    vec3 posWorld = (pc.invView * vec4(posView, 1.0)).xyz;

    vec3 viewDir = normalize(pc.cameraPos - posWorld);

    vec3 lighting = vec3(0.0);

    // Simple loop over lights (assumes count <= some small max, e.g. 32)
    for (int i = 0; i < lightBuffer.lights.length(); ++i) {
        Light light = lightBuffer.lights[i];
        vec3 lightDir = normalize(light.position - posWorld);
        float dist = length(light.position - posWorld);
        float attenuation = light.intensity / (dist * dist);

        float NdotL = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = albedo * light.color * NdotL * attenuation;

        // Simple specular (Blinn-Phong)
        vec3 halfVec = normalize(lightDir + viewDir);
        float specAngle = max(dot(normal, halfVec), 0.0);
        float roughness = roughnessMetallic.r;
        float specular = pow(specAngle, 16.0 * (1.0 - roughness)); // roughness affects shininess

        vec3 specularColor = light.color * specular * attenuation;

        lighting += diffuse + specularColor;
    }

    outColor = vec4(lighting, 1.0);
}

