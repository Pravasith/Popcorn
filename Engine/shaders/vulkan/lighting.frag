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
    // Depth is 0->1 enforced (NDC is [0, 1])
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPosH = camera.invViewProj * clipPos;
    return worldPosH.xyz / worldPosH.w;
}

float PointLightAttenuation(float dist) {
    float x = dist, x2 = x*x, x3 = x2*x;
    // return 1. / (x2 + 1.); // +1 to avoid div by 0
    return 1. / (.4*x3 + (-.8)*x2 + .3*x + 1.1);
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

    for (uint i = 0; i < lights.length(); ++i) {
        LightUniform light = lights[i];
        float attenuation = 1.0;
        vec3 lightColor = light.color * light.intensity;
        vec3 lightVec = light.position - worldPos;
        float dist = length(lightVec);

        if (light.lightType == 0.0) {
            // Point light
            attenuation = PointLightAttenuation(dist);
        } else if (light.lightType == 1.) {
            // Spot light
            attenuation = PointLightAttenuation(dist);

            // Angle between fragment direction and spotlight axis
            float cosTheta = dot(normalize(lightVec),
                    normalize(-light.direction));
            float cosInner = cos(light.innerConeAngle);
            float cosOuter = cos(light.outerConeAngle);

            // Smooth interpolation between inner/outer cone
            float spotEffect = clamp((cosTheta - cosOuter) /
                    (cosInner - cosOuter), 0.0, 1.0);

            attenuation *= spotEffect;
        } else if (light.lightType == 2.) {
            // Directional light
            lightVec = -light.direction;
            // take N dot L for Dir light
        }

        // Blinn-Phong
        vec3 viewVec = camera.camPos - worldPos;
        vec3 halfVec = lightVec + viewVec;

        // worldPos
        float NdotL = max(dot(normal, normalize(lightVec)), 0.0);
        float NdotH = max(dot(normal, normalize(halfVec)), 0.0);

        vec3 diffuse = albedo * lightColor * NdotL;

        // Blinn–Phong specular
        float shininess = 32.0 * 2.0;  // tweak for sharper/broader highlights
        float specularStrength = 0.4;  // tweak intensity
        vec3 specular = lightColor * pow(NdotH, shininess) * specularStrength;

        finalColor += (diffuse + specular) * attenuation;
    }

    float ambient = 0.01;

    finalColor += albedo * ambient; // Ambient

    outColor = vec4(finalColor, 1.0);
}
