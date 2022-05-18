// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    constexpr Viewport::operator VkViewport() const noexcept {
        return { .x        = position.x,
                 .y        = position.y,
                 .width    = extent.width,
                 .height   = extent.height,
                 .minDepth = depth.x,
                 .maxDepth = depth.y };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr Scissor::operator VkRect2D() const noexcept {
        return { .offset = { offset.x, offset.y }, .extent = { extent.width, extent.height } };
    }
} // namespace stormkit::gpu
