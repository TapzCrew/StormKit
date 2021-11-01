#version 460 core

#pragma shader_stage(fragment)

layout (location = 0) in vec3 in_texcoords;

layout (location = 0) out vec4 out_color;

layout (set = 0, binding = 0) uniform samplerCube cube_map;

layout(push_constant) uniform PushConstants {
   layout (offset = 64) float roughness;
   layout (offset = 68) uint num_samples;
} push_constants;

const float M_PI = 3.1415926535897932384626433832795;

float random(vec2 co) {
        float a = 12.9898f;
        float b = 78.233f;
        float c = 43758.5453f;
        float dt= dot(co.xy ,vec2(a,b));
        float sn= mod(dt,3.14f);
        return fract(sin(sn) * c);
}

vec2 hammersley2d(uint i, uint N) {
        uint bits = (i << 16u) | (i >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        float rdi = float(bits) * 2.3283064365386963e-10;
        return vec2(float(i) /float(N), rdi);
}

vec3 importanceSample_GGX(vec2 Xi, float roughness, vec3 normal) {
        float alpha = roughness * roughness;
        float phi = 2.0 * M_PI * Xi.x + random(normal.xz) * 0.1;
        float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

        vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
        vec3 tangentX = normalize(cross(up, normal));
        vec3 tangentY = normalize(cross(normal, tangentX));

        return normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
}

float D_GGX(float dotNH, float roughness) {
        float alpha = roughness * roughness;
        float alpha2 = alpha * alpha;
        float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
        return (alpha2)/(M_PI * denom*denom);
}

vec3 prefilterEnvMap(vec3 R, float roughness) {
        vec3 N = R;
        vec3 V = R;
        vec3 color = vec3(0.0);
        float totalWeight = 0.0;
        float envMapDim = float(textureSize(cube_map, 0).s);
        for(uint i = 0u; i < push_constants.num_samples; i++) {
                vec2 Xi = hammersley2d(i, push_constants.num_samples);
                vec3 H = importanceSample_GGX(Xi, roughness, N);
                vec3 L = 2.0 * dot(V, H) * H - V;
                float dotNL = clamp(dot(N, L), 0.0, 1.0);
                if(dotNL > 0.0) {
                        float dotNH = clamp(dot(N, H), 0.0, 1.0);
                        float dotVH = clamp(dot(V, H), 0.0, 1.0);

                        float pdf = D_GGX(dotNH, roughness) * dotNH / (4.0 * dotVH) + 0.0001;
                        float omegaP = 4.0 * M_PI / (6.0 * envMapDim * envMapDim);
                        float omegaS = 1.0 / (float(push_constants.num_samples) * pdf);
                        float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(omegaS / omegaP) + 1.0, 0.0f);
                        color += textureLod(cube_map, L, mipLevel).rgb * dotNL;
                        totalWeight += dotNL;

                }
        }
        return (color / totalWeight);
}


void main() {
    vec3 N = normalize(in_texcoords);
    out_color = vec4(prefilterEnvMap(N, push_constants.roughness), 1.0);
}
