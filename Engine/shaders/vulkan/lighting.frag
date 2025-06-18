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

    // float ndcZ = depth * 2.0 - 1.0; // Convert to NDC z
    // vec4 clipPos = vec4(uv * 2.0 - 1.0, ndcZ, 1.0);

vec3 ReconstructWorldSpace(vec2 uv, float depth) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPosH = camera.invViewProj * clipPos;
    return worldPosH.xyz / worldPosH.w;
}

void main() {
    float depth = texture(depthTex, fragUV).r;

    vec3 albedo = texture(albedoTex, fragUV).rgb;
    vec3 normal = normalize(texture(normalTex, fragUV).xyz * 2.0 - 1.0);
    vec2 rm = texture(roughMetalTex, fragUV).rg;

    float roughness = rm.r;
    float metallic = rm.g;

    vec3 worldPos = ReconstructWorldSpace(fragUV, depth);

    vec3 finalColor = vec3(0.0);

    // // tweak these to suit your 1000×1000 scene
    // const float c = 1.0;
    // const float l = 0.01;
    // const float q = 0.00002;

    for (uint i = 0; i < lights.length(); ++i) {
        LightUniform light = lights[i];
        vec3 lightVec;
        float attenuation = 1.0;

        if (light.lightType == 0.0) {
            // Point light
            lightVec = light.position - worldPos;
            float dist = length(lightVec);
            // if (dist > 50.0)      // dead outside your desired radius
            //     continue;
            attenuation = 1.0 / (dist * dist + 1.0); // Avoid div by zero
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
        // vec3 diffuse = albedo * lightColor;

        finalColor += diffuse * attenuation * 0.1;
    }

    finalColor += albedo * 0.05; // Ambient
    outColor = vec4(finalColor, 1.0);

    // float NdotL = max(dot(normal, lightVec), 0.0);
    // outColor = vec4(vec3(NdotL), 1.0);
}

// float near = 0.1;
// float far = 1000.0;
//
// float d = texture(depthTex, fragUV).r;
// float linearDepth = (near * far) /
//     (far - d * (far - near));
// float dVis = clamp(linearDepth / far, 0.0, 1.0);
// outColor = vec4(vec3(1.0 - dVis), 1.0);  // Near = white

// float near = 0.1;
// float far = 1000.0;
//
// float d = texture(depthTex, fragUV).r;
// float linearDepth = (near * far) /
//     (far - d * (far - near));
// float dVis = clamp(linearDepth / far, 0.0, 1.0);
// outColor = vec4(vec3(1.0 - dVis), 1.0);  // Near = white



    // float d = texture(depthTex, fragUV).r;
    // float c = pow(1.0 - d, 40.0); // exaggerates contrast near the camera
    // outColor = vec4(vec3(c), 1.0);

    // vec3 testPos = ReconstructWorldSpace(fragUV, depth);
    // outColor = vec4((testPos + vec3(10.0)) / 20.0, 1.0); // Normalized for view

    // float NdotL = max(dot(normal, lightVec), 0.0);
    // outColor = vec4(vec3(NdotL), 1.0);

    // outColor = vec4(vec3(texture(depthTex, fragUV).r), 1.0);
    // outColor = vec4(vec3(texture(albedoTex, fragUV)), 1.0);
