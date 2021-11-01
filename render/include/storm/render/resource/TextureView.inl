// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const Texture &TextureView::texture() const noexcept {
        // STORMKIT_EXPECTS(m_texture != nullptr);

        return *m_texture;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    TextureViewType TextureView::type() const noexcept { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    const TextureSubresourceRange &TextureView::subresourceRange() const noexcept {
        return m_subresource_range;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::ImageView TextureView::vkImageView() const noexcept {
        STORMKIT_EXPECTS(m_vk_image_view);
        return *m_vk_image_view;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    TextureView::operator vk::ImageView() const noexcept { return vkImageView(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::ImageView TextureView::vkHandle() const noexcept { return vkImageView(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 TextureView::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkImageView_T *());
    }
} // namespace storm::render
