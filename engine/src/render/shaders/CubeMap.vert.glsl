#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) out vec3 out_texcoord;

layout(set = 0, binding = 0, std140) uniform Camera {
    vec4 position;
    mat4 projection;
    mat4 view;
} camera;

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

out gl_PerVertex  {
    vec4 gl_Position;
};

void main()
{
    vec3 position = positions[gl_VertexIndex];

    out_texcoord = position;
    out_texcoord.x *= -1.0;

    vec4 out_position = camera.projection * mat4(mat3(camera.view)) * vec4(position, 1.0);
    gl_Position = out_position.xyww;
}
