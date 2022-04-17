// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::extent() const noexcept -> const core::ExtentU & {
        STORMKIT_EXPECTS(m_image);

        return m_extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::format() const noexcept -> PixelFormat {
        STORMKIT_EXPECTS(m_image);

        return m_format;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::type() const noexcept -> ImageType {
        STORMKIT_EXPECTS(m_image);

        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::samples() const noexcept -> SampleCountFlag {
        STORMKIT_EXPECTS(m_image);

        return m_samples;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::layers() const noexcept -> core::UInt32 {
        STORMKIT_EXPECTS(m_image);

        return m_layers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::faces() const noexcept -> core::UInt32 {
        STORMKIT_EXPECTS(m_image);

        return m_faces;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::mipLevels() const noexcept -> core::UInt32 {
        STORMKIT_EXPECTS(m_image);

        return m_mip_levels;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::usage() const noexcept -> ImageUsageFlag {
        STORMKIT_EXPECTS(m_image);

        return m_usage;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Image::operator VkImage() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Image::vkHandle() const noexcept -> VkImage {
        STORMKIT_EXPECTS(m_image);

        return m_image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Image::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_IMAGE;
    }
} // namespace stormkit::render
