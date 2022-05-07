// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<Texture> Surface::textures() noexcept { return m_textures; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline storm::core::span<const Texture> Surface::textures() const noexcept {
        return m_textures;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PixelFormat Surface::pixelFormat() const noexcept { return m_pixel_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Surface::bufferingCount() const noexcept { return m_buffering_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt32 Surface::textureCount() const noexcept { return m_image_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Surface::needRecreate() const noexcept { return m_need_recreate; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Extentu &Surface::extent() const noexcept { return m_extent; }
} // namespace storm::render
