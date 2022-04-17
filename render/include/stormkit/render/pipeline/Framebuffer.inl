// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Framebuffer::extent() const noexcept -> const core::ExtentU & { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Framebuffer::attachments() const noexcept -> std::span<const ImageViewConstRef> {
        return m_attachments;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Framebuffer::operator VkFramebuffer() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Framebuffer::vkHandle() const noexcept -> VkFramebuffer {
        STORMKIT_EXPECTS(m_framebuffer);

        return m_framebuffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Framebuffer::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_FRAMEBUFFER;
    }
} // namespace stormkit::render
