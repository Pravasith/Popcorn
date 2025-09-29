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
layout(set = 1, binding = 1) uniform sampler2D depthTex;
layout(set = 1, binding = 2) uniform sampler2D normalTex;

// Reconstruct view-space position from depth
vec3 ReconstructViewPos(vec2 uv, float depth, mat4 invViewProj) {
    // NDC coords in [-1, 1]
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewPos = invViewProj * ndc;
    return viewPos.xyz / viewPos.w;
}

void main() {
    // Scene color
    vec3 sceneColor = texture(lightTex, fragUV).rgb;

    // Depth sample
    float depth = texture(depthTex, fragUV).r;

    // Reconstruct view-space position + distance
    vec3 viewPos = ReconstructViewPos(fragUV, depth, camera.invViewProj);
    float distance = length(viewPos);

    // Hardcoded fog params
    const vec3 fogColor   = vec3(0.6, 0.7, 0.8); // light bluish-gray fog
    const float fogDensity = 0.0025;             // tweak for intensity
    const float fogNear = 5.0;   // fog starts here
    const float fogFar  = 10.0;  // fully fogged here

    // // Exponential fog
    // float fogFactor = 1.0 - exp(-pow(distance * fogDensity, 2.0));
    // fogFactor = clamp(fogFactor, 0.0, 1.0);

     // Linear fog
    float fogFactor = (distance - fogNear) / (fogFar - fogNear);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Blend scene with fog
    vec3 finalColor = mix(sceneColor, fogColor, fogFactor);
    outColor = vec4(finalColor, 1.0);
}
