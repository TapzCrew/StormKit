#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(fragment)

#define MAX_MATERIAL_COUNT 255
#define MAX_TEXTURE_COUNT 255

struct Material {
    vec4 diffuse;
    vec4 ambiant;
    vec4 specular;

    int diffuse_map_index;
    int ambiant_map_index;
    int specular_map_index;
};

struct DirectionalLight {
    vec4 direction;

    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;

layout(location = 0) out vec4 out_color;

/*layout(set = 2, binding = 0, std140) uniform Materials {
    Material materials[MAX_MATERIAL_COUNT];
};

layout(set = 2, binding = 1) uniform sampler2D textures[MAX_TEXTURE_COUNT];*/

/*layout(set = 0, binding = 1, std140) uniform DirectionalLight directional_light;

const float ambiant_strength = 0.3;

void main() {
    /*vec3 normal = normalize(frag_normal);
    vec3 light_direction = normalize(-directional_light.direction);

    vec3 color = vec3(1.f, 0.84f, 0.f);

    float diffuse_coef = max(dot(normal, light_direction), 0.f);
    vec3 light_diffuse_color = diffuse_coef * directional_light.diffuse;

    vec3 light_ambiant_color = ambiant_strength * directional_light.ambiant;

    out_color = vec4((light_ambiant_color + light_diffuse_color) * color, 1.f);*/

void main() {
    out_color = vec4(in_normal.x, in_normal.y, in_normal.x, 1.f);//materials[material_index];
}
