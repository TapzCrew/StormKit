#version 460 core

#pragma shader_stage(fragment)

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_tangent;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0, std140) uniform Camera {
    vec4 position;
    mat4 projection;
    mat4 view;
} camera;

layout(set = 1, binding = 0) uniform sampler2D brdf;
layout(set = 1, binding = 1) uniform samplerCube irradience;
layout(set = 1, binding = 2) uniform samplerCube prefiltered_env;
layout(set = 1, binding = 3) uniform SceneData {
   float exposure;
   float gamma;
   float prefiltered_cube_mip_levels;
} scene_data;

layout(set = 4, binding = 0) uniform sampler2D albedo_map;
layout(set = 4, binding = 1) uniform sampler2D normal_map;
layout(set = 4, binding = 2) uniform sampler2D metallic_map;
layout(set = 4, binding = 3) uniform sampler2D roughness_map;
layout(set = 4, binding = 4) uniform sampler2D ambiant_occlusion_map;
layout(set = 4, binding = 5) uniform sampler2D emissive_map;

layout(set = 4, binding = 6, std140) uniform Material {
    vec4 albedo_factor;
    float metallic_factor;
    float roughness_factor;
    float ambiant_occlusion_factor;
    uint   debug_index;
    vec4 emissive_factor;
} material;

vec4 sRGBtoLinear(vec4 srgb_in);
vec3 getNormal();
vec3 diffuseColor(vec3 diffuse);
vec3 specularReflection(vec3 reflectance0, vec3 reflectance90, float VdotH);
float geometricOcclusion(float NdotL, float NdotV, float alpha_roughness);
float microfacetDistribution(float alpha_roughness, float NdotH);
vec4 tonemap(vec4 color);
vec3 getIBLContribution(float roughness, float NdotV, vec3 diffuse_color, vec3 specular_color, vec3 n, vec3 reflection);

const vec3 light_color     = vec3(1.f, 1.f, 1.f);
const vec3 light_direction = vec3(0,
                                  0,
                                  2.f);

const float pi = 3.141592653589f;
const float min_roughness = 0.04f;

const float pbr_workflow_metallic_roughness = 0.f;
const float pbr_workflow_specular_glosiness = 1.f;

const vec3 f0 = vec3(0.04f);

const float exposure = 4.5f;
const float gamma = 2.2f;

void main() {
    vec4 albedo = material.albedo_factor * sRGBtoLinear(texture(albedo_map, in_texcoord));

    float metallic = material.metallic_factor * texture(metallic_map, in_texcoord).b;

    float roughness = material.roughness_factor * texture(metallic_map, in_texcoord).g;

    vec3 emissive = material.emissive_factor.rgb * sRGBtoLinear(texture(emissive_map, in_texcoord)).rgb;

    float ambiant_occlusion = texture(ambiant_occlusion_map, in_texcoord).r;

    vec3 diffuse_color = albedo.rgb * (vec3(1.f) - f0);
    diffuse_color *= 1.f - metallic;

    float alpha_roughness = roughness * roughness;

    vec3 specular_color = mix(f0, albedo.rgb, metallic);

    float reflectance = max(max(specular_color.r, specular_color.g), specular_color.b);

    float reflectance90 = clamp(reflectance * 25.f, 0.f, 1.f);

    vec3 specular_env_r0 = specular_color.rgb;
    vec3 specular_env_r90 = vec3(1.f) * reflectance90;

    vec3 n = getNormal();
    vec3 v = normalize(camera.position.xyz - in_position);
    vec3 l = normalize(light_direction.xyz);
    vec3 h = normalize(l + v);

    vec3 reflection = -normalize(reflect(v, n));
    reflection.y *= -1.f;

    float NdotL = clamp(dot(n, l), 0.001f, 1.f);
    float NdotV = clamp(abs(dot(n, v)), 0.001f, 1.f);
    float NdotH = clamp(dot(n, h), 0.f, 1.f);
    float LdotH = clamp(dot(l, h), 0.f, 1.f);
    float VdotH = clamp(dot(v, h), 0.f, 1.f);

    vec3 F = specularReflection(specular_env_r0, specular_env_r90, VdotH);
    float G = geometricOcclusion(NdotL, NdotV, alpha_roughness);
    float D = microfacetDistribution(alpha_roughness, NdotH);

    vec3 diffuse_contribution = (1.f - F) * diffuseColor(diffuse_color);
    vec3 specular_contribution = F * G * D / (4.f * NdotL * NdotV);

    vec3 color = NdotL * light_color * (diffuse_contribution + specular_contribution);

    color += getIBLContribution(roughness, NdotV, diffuse_color, specular_color, n, reflection);

    color = mix(color, color * ambiant_occlusion, material.ambiant_occlusion_factor);
    color += emissive;

    out_color = vec4(color, albedo.a);

    if(material.debug_index > 0u) {
        if(material.debug_index <= 6u) {
            switch(material.debug_index) {
                case 1: out_color = texture(albedo_map, in_texcoord); break;
                case 2: out_color.rgb = texture(normal_map, in_texcoord).rgb; break;
                case 3: out_color.rgb = texture(metallic_map, in_texcoord).bbb; break;
                case 4: out_color.rgb = texture(roughness_map, in_texcoord).ggg; break;
                case 5: out_color.rgb = texture(ambiant_occlusion_map, in_texcoord).rrr; break;
                case 6: out_color.rgb = texture(emissive_map, in_texcoord).rgb; break;
            }

            out_color = sRGBtoLinear(out_color);
        } else {
            switch(material.debug_index) {
                case 7: out_color.rgb = diffuse_contribution; break;
                case 8: out_color.rgb = specular_contribution; break;
                case 9: out_color.rgb = F; break;
                case 10: out_color.rgb = vec3(G); break;
                case 11: out_color.rgb = vec3(D); break;
            }
        }
    }
}

vec4 sRGBtoLinear(vec4 srgb_in) {
    vec3 lin_out = pow(srgb_in.rgb, vec3(2.2));

    //vec3 bless   = step(vec3(0.04045), srgb_in.xyz);
    //vec3 lin_out = mix(srgb_in.rgb / vec3(12.92), pow((srgb_in.rgb + vec3(0.055)) / vec3(1.055), vec3(2.4)), bless);

    return srgb_in;//vec4(lin_out, srgb_in.a);
}

vec3 getNormal() {
    vec3 tangent_normal = texture(normal_map, in_texcoord).xyz * 2.f - 1.f;

    vec3 q1 = dFdx(in_position);
    vec3 q2 = dFdy(in_position);

    vec2 st1 = dFdx(in_texcoord);
    vec2 st2 = dFdy(in_texcoord);

    vec3 N = normalize(in_normal);
    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
    //vec3 T = normalize(in_tangent.xyz);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangent_normal);
}

vec3 diffuseColor(vec3 diffuse) {
    return diffuse / pi;
}

vec3 specularReflection(vec3 reflectance0, vec3 reflectance90, float VdotH) {
    return reflectance0 + (reflectance90 - reflectance0) * pow(clamp(1.f - VdotH, 0.f, 1.f), 5.f);
}

float geometricOcclusion(float NdotL, float NdotV, float alpha_roughness) {
    /*float alpha_roughness_pow_2 = pow(alpha_roughness, 2.f);

    float attenuation_L = 2.f * NdotL / (NdotL + sqrt(alpha_roughness_pow_2 + (1.f - alpha_roughness_pow_2)) * pow(NdotL, 2.f));
    float attenuation_V = 2.f * NdotV / (NdotV + sqrt(alpha_roughness_pow_2 + (1.f - alpha_roughness_pow_2)) * pow(NdotV, 2.f));
*/
    float r = alpha_roughness;

    float attenuation_L = 2.f * NdotL / (NdotL + sqrt(r * r + (1.f - r * r) * (NdotL * NdotL)));
    float attenuation_V = 2.f * NdotV / (NdotV + sqrt(r * r + (1.f - r * r) * (NdotV * NdotV)));

    return attenuation_L * attenuation_V;
}

float microfacetDistribution(float alpha_roughness, float NdotH) {
    float alpha_roughness_pow_2 = alpha_roughness * alpha_roughness;

    float f = (NdotH * alpha_roughness_pow_2 - NdotH) * NdotH + 1.f;

    return alpha_roughness_pow_2 / (pi * f * f);
}

vec3 Uncharted2Tonemap(vec3 color) {
        float A = 0.15f;
        float B = 0.50f;
        float C = 0.10f;
        float D = 0.20f;
        float E = 0.02f;
        float F = 0.30f;
        float W = 11.2f;

        return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

vec4 tonemap(vec4 color) {
    vec3 out_col = Uncharted2Tonemap(color.rgb * scene_data.exposure);

    out_col = out_col * (1.0f / Uncharted2Tonemap(vec3(11.2f)));

    return vec4(pow(out_col, vec3(1.0f / scene_data.gamma)), color.a);
}

vec3 getIBLContribution(float roughness, float NdotV, vec3 diffuse_color, vec3 specular_color, vec3 n, vec3 reflection) {
    float lod = (roughness * scene_data.prefiltered_cube_mip_levels);

    vec3 _brdf = (texture(brdf, vec2(NdotV, 1.f - roughness))).rgb;
    vec3 diffuse_light = sRGBtoLinear(tonemap(texture(irradience, n))).rgb;

    vec3 specular_light = sRGBtoLinear(tonemap(textureLod(prefiltered_env, reflection, lod))).rgb;

    vec3 diffuse = diffuse_light * diffuse_color;
    vec3 specular = specular_light * (specular_color * _brdf.x + _brdf.y);

    return diffuse + specular;
}
