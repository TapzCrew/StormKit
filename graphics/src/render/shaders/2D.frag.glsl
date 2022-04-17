#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(fragment)

layout(location = 0) in vec3 frag_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(frag_color, 1.0);
}
