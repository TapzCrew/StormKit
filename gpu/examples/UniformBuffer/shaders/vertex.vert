#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
} camera;

layout(set = 0, binding = 1, std140) uniform MeshData {
    mat4 model;
} mesh_data;

out gl_PerVertex  {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(position, 1.f);

    gl_Position = camera.projection * camera.view * mesh_data.model * p;

    frag_color = color;
}
