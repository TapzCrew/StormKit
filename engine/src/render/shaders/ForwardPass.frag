#version 460 core

#pragma shader_stage(fragment)

struct VSInput {
   vec3 normal;
   vec3 color;
   vec2 uv;
};

layout(location = 0) in VSInput vs_input;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(vs_input.color, 1.0);
}
