#version 460 core

#pragma shader_stage(fragment)

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D diffuse_map;

void main() {
    out_color = texture(diffuse_map, frag_uv);
}
