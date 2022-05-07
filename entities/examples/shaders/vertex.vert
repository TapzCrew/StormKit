#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) out vec2 frag_uv;

vec2 uvs[4] = vec2[](
    vec2(0.f, 0.f),
    vec2(1.f, 0.f),
    vec2(0.f, 1.f),
    vec2(1.f,  1.f)
);


vec2 positions[4] = vec2[](
    vec2(-1.f, -1.f),
    vec2(1.f,  -1.f),
    vec2(-1.f, 1.f),
    vec2(1.f,  1.f)
);

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(positions[gl_VertexIndex], 0.f, 1.f);

    gl_Position = p;

    frag_uv = uvs[gl_VertexIndex];
}
