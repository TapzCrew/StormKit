// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    core::Extentu Texture::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    PixelFormat Texture::format() const noexcept { return m_format; }

    /////////////////////////////////////
    /////////////////////////////////////
    TextureType Texture::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    SampleCountFlag Texture::samples() const noexcept { return m_samples; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Texture::layers() const noexcept { return m_layers; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Texture::faces() const noexcept { return m_faces; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Texture::mipLevels() const noexcept { return m_mip_levels; }

    /////////////////////////////////////
    /////////////////////////////////////
    TextureUsage Texture::usage() const noexcept { return m_usage; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Device &Texture::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Image Texture::vkImage() const noexcept {
        if (m_non_owning_texture) { return m_non_owning_texture; }

        STORMKIT_EXPECTS(m_vk_texture);
        return *m_vk_texture;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Texture::operator vk::Image() const noexcept { return vkImage(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Image Texture::vkHandle() const noexcept { return vkImage(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Texture::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkImage_T *());
    }
} // namespace storm::render
