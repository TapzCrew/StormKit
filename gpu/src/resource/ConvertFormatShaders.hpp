#pragma once

#include <string_view>

static constexpr auto CONVERT_R8_TO_RG8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	float input[];
};

layout (set = 0, binding = 1, rg16f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	float in_color  = input[index];
	vec2 out_color = vec2(in_color, 0.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };

static constexpr auto CONVERT_R8_TO_RGB8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	float input[];
};

layout (set = 0, binding = 1, rgb24f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	float in_color  = input[index];
	vec3 out_color = vec4(in_color, 0.f, 0.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };

static constexpr auto CONVERT_R8_TO_RGBA8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	float input[];
};

layout (set = 0, binding = 1, rgba32f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	float in_color  = input[index];
	vec4 out_color = vec4(in_color, 0.f, 0.f, 1.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };

static constexpr auto CONVERT_RG8_TO_RGB8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	vec2 input[];
};

layout (set = 0, binding = 1, rgb24f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	vec2 in_color  = input[index];
	vec3 out_color = vec4(in_color, 0.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };

static constexpr auto CONVERT_RG8_TO_RGBA8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	vec2 input[];
};

layout (set = 0, binding = 1, rgba32f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	vec2 in_color  = input[index];
	vec4 out_color = vec4(in_color, 0.f, 1.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };

static constexpr auto CONVERT_RGB8_TO_RGBA8 = std::string_view { R"(
#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(std430, set = 0, binding = 0) buffer In {
	vec3 input[];
};

layout (set = 0, binding = 1, rgba32f) uniform image2D result;

layout (local_size_x = 256) in;

layout (binding = 2) uniform Parameters  {
	int size;
	int width;
} parameters;


void main() {
	uint index = gl_GlobalInvocationID.x;
	if (index >= parameters.size) {
		return;
	}

	vec3 in_color  = input[index];
	vec4 out_color = vec4(in_color, 1.f);

	int row = gl_GlobalInvocationID.x / parameters.width;
	int col = gl_GlobalInvocationID.x % parameters.width;

	ivec2 image_write_location = ivec2(row, col);

	imageStore(result, image_write_location, out_color);
})" };
