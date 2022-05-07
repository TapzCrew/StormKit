// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/Color.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Types.hpp>

namespace storm::render {
    struct Viewport {
        core::Vector2f position;
        core::Extentf extent;
        core::Vector2f depth;
    };

    struct Scissor {
        core::Vector2i offset;
        core::Extentu extent;
    };

    struct ClearColor {
        core::RGBColorF color = storm::core::RGBColorDef::Silver<float>;
    };

    struct ClearDepthStencil {
        float depth          = 1.f;
        core::UInt32 stencil = 0;
    };

    using ClearValue = std::variant<ClearColor, ClearDepthStencil>;
    using SpirvID    = core::UInt32;
} // namespace storm::render

HASH_FUNC(storm::render::Viewport)
HASH_FUNC(storm::render::Scissor)
