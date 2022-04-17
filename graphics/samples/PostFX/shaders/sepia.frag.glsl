#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(fragment)

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D input_color;

const vec2 direction = vec2(0.f, 1.f);

const mat4 sepia = mat4(
        0.3588, 0.7044, 0.1368, 0,
        0.2990, 0.5870, 0.1140, 0,
        0.2392, 0.4696, 0.0912 ,0,
        0,0,0,0);

void main() {
    out_color = texture(input_color, frag_uv) * sepia;
}
