#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 invViewProj;
    vec3 camPos;
} camera;

layout(set = 1, binding = 0) uniform sampler2D lightTex;
layout(set = 1, binding = 1) uniform sampler2D depthTex;
layout(set = 1, binding = 2) uniform sampler2D normalTex;

vec3 ReconstructWorldSpace(vec2 uv, float depth) {
    // Depth is 0->1 enforced
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth, 1.0);
    vec4 worldPosH = camera.invViewProj * clipPos;
    return worldPosH.xyz / worldPosH.w;
}

void main() {
    // Scene color
    vec3 sceneColor = texture(lightTex, fragUV).rgb;

    // Depth sample
    float depth = texture(depthTex, fragUV).r;

    // Reconstruct view-space position + distance
    vec3 worldPos = ReconstructWorldSpace(fragUV, depth);
    vec3 viewPos  = (camera.view * vec4(worldPos, 1.0)).xyz;

    float distance = length(viewPos);

    const float fogDimmer = 0.8;

    // Hardcoded fog params
    const vec3 fogColor  = fogDimmer *
        // vec3(0.6 , 0.7, 0.8); // light bluish-gray fog
        // vec3(0.9 , 0.2, 0.3); // light bluish-gray fog
        // vec3(0.043, 0.106, 0.188); // dark cyan-blue
        // vec3(0.039, 0.098, 0.141); // darker cyan-blue
        vec3(0.000, 0.059, 0.110); // darkest cyan

    // const float fogDensity = 0.0025;             // tweak for intensity
    const float fogNear = 12;   // fog starts here
    const float fogFar  = 330.0;  // fully fogged here

    // // Exponential fog
    // float fogFactor = 1.0 - exp(-pow(distance * fogDensity, 2.0));
    // fogFactor = clamp(fogFactor, 0.0, 1.0);

     // Linear fog
    float fogFactor = (distance - fogNear) / (fogFar - fogNear);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Blend scene with fog
    vec3 finalColor = mix(sceneColor, fogColor, fogFactor);


    // Typical brightness cap flow in engines:
    //
    // Lighting pass -> HDR colors.
    // Tone map / brightness cap -> compress into LDR (≤ 0.95).
    // Emissive/glow pass -> add glow materials on top without capping.
    // Bloom -> optional, to spread glow.
    // Final output -> gamma correct and write to swapchain.

    // Reinhard tonemapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    outColor = vec4(finalColor, 1.0);
}
