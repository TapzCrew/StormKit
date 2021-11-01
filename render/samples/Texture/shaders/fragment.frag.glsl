#version 460 core

#pragma shader_stage(fragment)

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 1) uniform sampler2D diffuse_map;

void main() {
    vec4 color = texture(diffuse_map, frag_uv);
    //if(color.a < 1.f)
        //discard;

    out_color = color;
}
