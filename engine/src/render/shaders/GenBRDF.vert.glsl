#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) out vec2 out_texcoords;

void main() {
    out_texcoords = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);

    gl_Position = vec4(out_texcoords * 2.f - 1.f, 0.f, 1.f);
}
