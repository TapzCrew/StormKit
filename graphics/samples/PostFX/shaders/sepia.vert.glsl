#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(vertex)

layout(location = 0) out vec2 frag_uv;

void main() {
    float x = float(((uint(gl_VertexIndex) + 2u) / 3u)%2u);
    float y = float(((uint(gl_VertexIndex) + 1u) / 3u)%2u);

    gl_Position = vec4(-1.0f + x * 2.0f, -1.0f + y * 2.0f, 0.0f, 1.0f);

    frag_uv = vec2(x, y);

    gl_Position.y = -gl_Position.y;
}
