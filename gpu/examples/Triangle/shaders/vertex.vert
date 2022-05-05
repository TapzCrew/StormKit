#version 460 core

#pragma shader_stage(vertex)

layout(location = 0) out vec3 frag_color;

vec3 colors[3] = vec3[](
    vec3(1.f, 0.f, 0.f),
    vec3(0.f, 1.f, 0.f),
    vec3(0.f, 0.f, 1.f)
);


vec2 positions[3] = vec2[](
    vec2(1.f, 1.f),
    vec2(-1.f, 1.f),
    vec2(0.f,  -1.f)
);

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 p = vec4(positions[gl_VertexIndex], 0.f, 1.f);

    gl_Position = p;

    frag_color = colors[gl_VertexIndex];
}
