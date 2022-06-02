// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::device() const noexcept -> const Device & {
        STORMKIT_EXPECTS(m_device);
        return *m_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::images() noexcept -> std::span<Image> { return m_images; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::images() const noexcept -> std::span<const Image> { return m_images; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::pixelFormat() const noexcept -> PixelFormat { return m_pixel_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::bufferingCount() const noexcept -> core::UInt32 {
        return m_buffering_count;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::imageCount() const noexcept -> core::UInt32 { return m_image_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::needRecreate() const noexcept -> bool { return m_need_recreate; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Surface::extent() const noexcept -> const core::ExtentU & { return m_extent; }
} // namespace stormkit::gpu
