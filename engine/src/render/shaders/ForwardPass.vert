#version 460 core

#pragma shader_stage(vertex)

struct FSInput {
   vec3 normal;
   vec3 color;
   vec2 uv;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 uv;

layout(location = 0) out FSInput fs_input;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(position, 1.f);

    fs_input.normal = normal;
    fs_input.color = color;
    fs_input.uv = uv;

    gl_Position = p;
}
