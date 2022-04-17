// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ImageView::image() const noexcept -> const Image & {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_image);

        return *m_image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ImageView::type() const noexcept -> ImageViewType {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_image);
        STORMKIT_EXPECTS(m_image_view);

        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ImageView::subresourceRange() const noexcept -> const ImageSubresourceRange & {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_image);
        STORMKIT_EXPECTS(m_image_view);

        return m_subresource_range;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline ImageView::operator VkImageView() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ImageView::vkHandle() const noexcept -> VkImageView {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_image);
        STORMKIT_EXPECTS(m_image_view);

        return m_image_view;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto ImageView::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_IMAGE_VIEW;
    }
} // namespace stormkit::render
