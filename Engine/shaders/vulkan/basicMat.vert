#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    mat4 invViewProj;
} camera;

layout(set = 2, binding = 0) uniform ObjectUBO {
    mat4 modelMatrix;
    mat4 normalMatrix;
} object;

void main() {
    // World space
    vec4 vertexPos = object.modelMatrix * vec4(inPosition, 1.0);
    // Clip space position
    gl_Position = camera.proj * camera.view * vertexPos;

    fragPosition = vertexPos.xyz; // TEMP_DEBUG: not used (fragPos)
    fragNormal = normalize(mat3(object.normalMatrix) * inNormal);
    fragUV = inUV;
}
