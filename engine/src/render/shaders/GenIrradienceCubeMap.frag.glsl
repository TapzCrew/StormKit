#version 460 core

#pragma shader_stage(fragment)

layout (location = 0) in vec3 in_texcoords;

layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform samplerCube cube_map;

layout(push_constant) uniform PushConstants {
   layout (offset = 64) float delta_phi;
   layout (offset = 68) float delta_theta;
} push_constants;

const float M_PI = 3.1415926535897932384626433832795;
const float TWO_PI = M_PI * 2.f;
const float HALF_PI = M_PI * 0.5f;

void main() {
    vec3 N = normalize(in_texcoords);
    vec3 up = vec3(0.f, 1.f, 0.f);
    vec3 right = normalize(cross(up, N));
    up = cross(N, right);

    vec3 color = vec3(0.f);
    uint sample_count = 0u;

    for(float phi = 0.f; phi < TWO_PI; phi += push_constants.delta_phi) {
        for(float theta = 0.f; theta < HALF_PI; theta += push_constants.delta_theta) {
            vec3 temp_vec = cos(phi) * right + sin(phi) * up;
            vec3 sample_vector = cos(theta) * N + sin(theta) * temp_vec;

            color += texture(cube_map, sample_vector).rgb * cos(theta) * sin(theta);

            ++sample_count;
        }
    }

    out_color = vec4(M_PI * color / float(sample_count), 1.f);
}
