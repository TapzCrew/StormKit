#version 460 core

#pragma shader_stage(fragment)

layout(location = 0) in vec3 in_texcoord;

layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform samplerCube cube_map;

layout(set = 1, binding = 0) uniform sampler2D brdf;
layout(set = 1, binding = 1) uniform samplerCube irradience;
layout(set = 1, binding = 2) uniform samplerCube prefiltered_env;
layout(set = 1, binding = 3) uniform SceneData {
   float exposure;
   float gamma;
   float prefiltered_cube_mip_levels;
} scene_data;


void main() {
    out_color = texture(cube_map, in_texcoord);
}
