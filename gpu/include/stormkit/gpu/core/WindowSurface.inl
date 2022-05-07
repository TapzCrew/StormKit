// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "WindowSurface.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowSurface::operator VkSurfaceKHR() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowSurface::vkHandle() const noexcept -> VkSurfaceKHR {
        STORMKIT_EXPECTS(m_surface);

        return m_surface;
    }
} // namespace stormkit::gpu
