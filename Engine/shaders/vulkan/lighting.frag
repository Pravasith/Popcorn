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
    vec3 camPos;
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
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPosH = camera.invViewProj * clipPos;
    return worldPosH.xyz / worldPosH.w;
}

float PointLightAttenuation(float dist) {
    return 1.0 / (dist * dist + 1.0);
}

void main() {

    float depth = texture(depthTex, fragUV).r;

    vec3 albedo = texture(albedoTex, fragUV).rgb;
    vec3 normal = normalize(texture(normalTex, fragUV).xyz * 2.0 - 1.0);
    vec2 rm = texture(roughMetalTex, fragUV).rg;

    float roughness = clamp(rm.r, 0.04, 1.0);
    float metallic = rm.g;

    vec3 worldPos = ReconstructWorldSpace(fragUV, depth);

    vec3 V = normalize(camera.camPos - worldPos);

    vec3 finalColor = vec3(0.0);

    for (uint i = 0; i < lights.length(); ++i) {
        LightUniform light = lights[i];

        vec3 lightVec = light.position - worldPos;
        float dist = length(lightVec);
        vec3 L = normalize(lightVec);

        float attenuation = 1.0;

        if (light.lightType == 0.0) {
            attenuation = PointLightAttenuation(dist);
        }

        else if (light.lightType == 1.0) {
            attenuation = PointLightAttenuation(dist);

            float cosTheta = dot(L, normalize(-light.direction));
            float cosInner = cos(light.innerConeAngle);
            float cosOuter = cos(light.outerConeAngle);

            float spotEffect = clamp((cosTheta - cosOuter) /
                    (cosInner - cosOuter), 0.0, 1.0);
            attenuation *= spotEffect;
        }

        else if (light.lightType == 2.0) {
            L = normalize(-light.direction);
            dist = 1.0;
            attenuation = 1.0;
        }

        vec3 lightColor = light.color * light.intensity;

        float NdotL = max(dot(normal, L), 0.0);

        if (NdotL <= 0.0)
            continue;

        vec3 H = normalize(L + V);
        float NdotH = max(dot(normal, H), 0.0);

        // Roughness to shininess
        float shininess = 2.0 + pow(1.0 - roughness, 6.0) * 128.0;

        // Base reflectivity (dielectrics ~4%)
        vec3 F0 = mix(vec3(0.04), albedo, metallic);

        // Specular (Blinn-Phong)
        vec3 specular = F0 * pow(NdotH, shininess);

        // Diffuse (reduced by specular energy)
        vec3 diffuse = (1.0 - F0) * albedo * NdotL;

        finalColor += (diffuse + specular) * lightColor * attenuation;
    }

    // Ambient
    float ambient = 0.03;
    finalColor += albedo * ambient;

    outColor = vec4(finalColor, 1.0);
}
