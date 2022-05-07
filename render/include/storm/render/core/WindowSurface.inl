// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::SurfaceKHR WindowSurface::vkSurface() const noexcept {
        STORMKIT_EXPECTS(m_vk_surface);
        return *m_vk_surface;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowSurface::operator vk::SurfaceKHR() const noexcept { return vkSurface(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::SurfaceKHR WindowSurface::vkHandle() const noexcept { return vkSurface(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 WindowSurface::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSurfaceKHR_T *());
    }
} // namespace storm::render
