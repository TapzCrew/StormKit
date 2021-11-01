// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const core::Extentu &Framebuffer::extent() const noexcept { return m_extent; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Device &Framebuffer::device() const noexcept { return m_render_pass->device(); }

    /////////////////////////////////////
    /////////////////////////////////////
    storm::core::span<const TextureViewConstPtr> Framebuffer::attachments() const noexcept {
        return m_attachments;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Framebuffer Framebuffer::vkFramebuffer() const noexcept {
        STORMKIT_EXPECTS(m_vk_framebuffer);
        return *m_vk_framebuffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Framebuffer::operator vk::Framebuffer() const noexcept { return vkFramebuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Framebuffer Framebuffer::vkHandle() const noexcept { return vkFramebuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Framebuffer::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkFramebuffer_T *());
    }
} // namespace storm::render
