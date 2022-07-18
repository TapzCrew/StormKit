// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>

namespace stormkit::gpu {
    struct ImageSubresourceRange {
        ImageAspectMaskFlag aspect_mask = ImageAspectMaskFlag::Color;

        core::UInt32 base_mip_level   = 0u;
        core::UInt32 level_count      = 1u;
        core::UInt32 base_array_layer = 0u;
        core::UInt32 layer_count      = 1u;
    };
} // namespace stormkit::gpu
