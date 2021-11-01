#version 460 core

#pragma shader_stage(vertex)

layout(push_constant) uniform PushConstants {
    layout (offset = 0) mat4 mvp;
} push_constants;

layout (location = 0) out vec3 out_texcoords;

const float size = 20.f;
vec3 positions[36] = vec3[](
    vec3(-size,  size, -size),
    vec3(-size, -size, -size),
    vec3(size, -size, -size),
    vec3(size, -size, -size),
    vec3(size,  size, -size),
    vec3(-size,  size, -size),

    vec3(-size, -size,  size),
    vec3(-size, -size, -size),
    vec3(-size,  size, -size),
    vec3(-size,  size, -size),
    vec3(-size,  size,  size),
    vec3(-size, -size,  size),

    vec3(size, -size, -size),
    vec3(size, -size,  size),
    vec3(size,  size,  size),
    vec3(size,  size,  size),
    vec3(size,  size, -size),
    vec3(size, -size, -size),

    vec3(-size, -size,  size),
    vec3(-size,  size,  size),
    vec3(size,  size,  size),
    vec3(size,  size,  size),
    vec3(size, -size,  size),
    vec3(-size, -size,  size),

    vec3(-size,  size, -size),
    vec3(size,  size, -size),
    vec3(size,  size,  size),
    vec3(size,  size,  size),
    vec3(-size,  size,  size),
    vec3(-size,  size, -size),

    vec3(-size, -size, -size),
    vec3(-size, -size,  size),
    vec3(size, -size, -size),
    vec3(size, -size, -size),
    vec3(-size, -size,  size),
    vec3(size, -size,  size)
);

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec3 position = positions[gl_VertexIndex];

    out_texcoords = position;

    gl_Position = push_constants.mvp * vec4(position, 1.f);
}
