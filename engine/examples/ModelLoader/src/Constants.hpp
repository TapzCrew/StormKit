// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/log/LogMacro.hpp>

#include <array>

#include <stormkit/core/Memory.hpp>

#include <stormkit/log/Logger.hpp>

#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/gpu/pipeline/VertexBindingDescription.hpp>
#include <stormkit/gpu/pipeline/VertexInputAttributeDescription.hpp>

#include <stormkit/engine/render/core/VertexArray.hpp>

static constexpr auto WINDOW_TITLE = "StormKit Model Loading Example";

static constexpr auto VERTICES = std::array {
    stormkit::engine::Vertex { .position = { 1.f, 1.f, 1.f }, .color = { 1.f, 0.f, 0.f } },
    stormkit::engine::Vertex { .position = { -1.f, 1.f, 1.f }, .color = { 0.f, 1.f, 0.f } },
    stormkit::engine::Vertex { .position = { 0.f, -1.f, 1.f }, .color = { 0.f, 0.f, 1.f } },
};

LOGGER("StormKit.Triangle");
