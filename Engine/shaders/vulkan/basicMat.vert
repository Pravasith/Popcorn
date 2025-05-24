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
} camera;

layout(set = 1, binding = 0) uniform ObjectUBO {
    mat4 model;
    mat4 normalMatrix;
} object;

void main() {
    vec4 vertexPos = object.model * vec4(inPosition, 1.0);

    gl_Position = camera.proj * camera.view * vertexPos;

    fragPosition = vertexPos.xyz;
    fragNormal = mat3(object.normalMatrix) * inNormal;
    fragUV = inUV;
}
