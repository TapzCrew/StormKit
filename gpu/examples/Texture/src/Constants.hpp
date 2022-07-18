// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/log/LogMacro.hpp>

#include <array>

/////////// - StormKit::Log - ///////////
#include <stormkit/core/Memory.hpp>

/////////// - StormKit::Log - ///////////
#include <stormkit/log/Logger.hpp>

/////////// - StormKit::Render - ///////////
#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/gpu/pipeline/VertexBindingDescription.hpp>
#include <stormkit/gpu/pipeline/VertexInputAttributeDescription.hpp>

struct Vertex {
    stormkit::core::Vector2f position;
    stormkit::core::Vector2f uv;
};

static constexpr auto WINDOW_TITLE  = "StormKit Texture Example";
static constexpr auto MESH_VERTICES = std::array {
    Vertex { .position = { -0.75f, -0.75f }, .uv = { 0.f, 0.f } },
    Vertex { .position = { 0.75f, -0.75f }, .uv = { 1.f, 0.f } },
    Vertex { .position = { -0.75f, 0.75f }, .uv = { 0.f, 1.f } },
    Vertex { .position = { 0.75f, 0.75f }, .uv = { 1.f, 1.f } },
};
static constexpr auto MESH_VERTEX_BUFFER_SIZE = sizeof(Vertex) * std::size(MESH_VERTICES);
static constexpr auto MESH_VERTEX_BINDING_DESCRIPTIONS =
    std::array { stormkit::gpu::VertexBindingDescription { .binding = 0,
                                                           .stride  = sizeof(Vertex) } };
static constexpr auto MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS = std::array {
    stormkit::gpu::VertexInputAttributeDescription { .location = 0,
                                                     .binding  = 0,
                                                     .format   = stormkit::gpu::Format::Float2,
                                                     .offset   = offsetof(Vertex, position) },
    stormkit::gpu::VertexInputAttributeDescription { .location = 1,
                                                     .binding  = 0,
                                                     .format   = stormkit::gpu::Format::Float2,
                                                     .offset   = offsetof(Vertex, uv) }
};

static constexpr auto SHADER_DATA = stormkit::core::makeStaticByteArray(
#include <shader.spv.h>
);

LOGGER("StormKit.Textures");
