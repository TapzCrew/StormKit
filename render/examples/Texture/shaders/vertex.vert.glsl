#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

layout(set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0) uniform MeshData {
    mat4 model;
} mesh_data;

out gl_PerVertex  {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(position, 0.f, 1.f);

    gl_Position = camera.projection * camera.view * mesh_data.model * p;
    frag_uv = uv;
}
