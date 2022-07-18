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

static constexpr auto VERTEX_SIZE =
    sizeof(stormkit::core::Vector2f) + sizeof(stormkit::core::Vector3f);
static constexpr auto WINDOW_TITLE            = "StormKit Triangle Example";
static constexpr auto MESH_VERTEX_BUFFER_SIZE = VERTEX_SIZE * 3;
static constexpr auto MESH_VERTEX_BINDING_DESCRIPTIONS =
    std::array { stormkit::gpu::VertexBindingDescription { .binding = 0, .stride = VERTEX_SIZE } };
static constexpr auto MESH_VERTEX_ATTRIBUTE_DESCRIPTIONS = std::array {
    stormkit::gpu::VertexInputAttributeDescription { .location = 0,
                                                     .binding  = 0,
                                                     .format   = stormkit::gpu::Format::Float2,
                                                     .offset   = 0 },
    stormkit::gpu::VertexInputAttributeDescription { .location = 1,
                                                     .binding  = 0,
                                                     .format   = stormkit::gpu::Format::Float3,
                                                     .offset   = sizeof(stormkit::core::Vector2f) }
};

static constexpr auto SHADER_DATA = stormkit::core::makeStaticByteArray(
#include <shader.spv.h>
);

LOGGER("StormKit.Triangle");
