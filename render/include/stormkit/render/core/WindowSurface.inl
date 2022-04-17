// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowSurface::operator VkSurfaceKHR() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowSurface::vkHandle() const noexcept -> VkSurfaceKHR {
        STORMKIT_EXPECTS(m_surface);

        return m_surface;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto WindowSurface::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_SURFACE_KHR;
    }
} // namespace stormkit::render
