#version 450

layout(location = 0) out vec2 uv;

const vec2 positions[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0),
    vec2(3.0, -1.0)
);

void main() {
    const vec2 inPos = positions[gl_VertexIndex];

    uv = inPos * 0.5 + 0.5; // convert from [-1,1] to [0,1]
    gl_Position = vec4(inPos, 0.0, 1.0);
}
