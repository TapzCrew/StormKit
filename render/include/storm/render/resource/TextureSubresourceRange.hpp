#pragma once

#include <storm/core/Math.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct TextureSubresourceRange {
        TextureAspectMask aspect_mask = TextureAspectMask::Color;

        core::UInt32 base_mip_level   = 0u;
        core::UInt32 level_count      = 1u;
        core::UInt32 base_array_layer = 0u;
        core::UInt32 layer_count      = 1u;
    };
} // namespace storm::render
