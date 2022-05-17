// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Renderer.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Renderer::setBuildFrameGraphCallback(BuildFrameGraphCallback callback) -> void {
        m_build_framegraph = std::move(callback);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Renderer::instance() const noexcept -> const gpu::Instance & { return *m_instance; }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Renderer::device() const noexcept -> const gpu::Device & { return *m_device; }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Renderer::surface() const noexcept -> const gpu::Surface & { return *m_surface; }
} // namespace stormkit::engine
