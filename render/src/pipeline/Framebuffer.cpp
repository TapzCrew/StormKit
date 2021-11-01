// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>

#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::Framebuffer(const RenderPass &render_pass,
                         core::Extentu extent,
                         TextureViewConstPtrArray attachments)
    : m_render_pass { &render_pass }, m_extent { std::move(extent) }, m_attachments { std::move(
                                                                          attachments) } {
    auto vk_attachments = std::vector<vk::ImageView> {};

    for (const auto &attachment : m_attachments) vk_attachments.emplace_back(*attachment);

    const auto create_info =
        vk::FramebufferCreateInfo {}
            .setRenderPass(*m_render_pass)
            .setAttachmentCount(gsl::narrow_cast<core::UInt32>(std::size(m_attachments)))
            .setPAttachments(std::data(vk_attachments))
            .setWidth(m_extent.width)
            .setHeight(m_extent.height)
            .setLayers(1);

    const auto &device = m_render_pass->device();
    m_vk_framebuffer   = device.createVkFramebuffer(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::~Framebuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
Framebuffer::Framebuffer(Framebuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Framebuffer &Framebuffer::operator=(Framebuffer &&) = default;
