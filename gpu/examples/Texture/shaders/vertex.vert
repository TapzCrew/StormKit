#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 frag_uv;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(position, 0.f, 1.f);

    gl_Position = p;

    frag_uv = uv;
}
