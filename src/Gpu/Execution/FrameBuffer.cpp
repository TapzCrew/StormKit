// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

import :Execution.RenderPass;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::FrameBuffer(const RenderPass& render_pass,
                             const core::math::ExtentU& extent,
                             std::vector<core::NakedRef<const ImageView>> attachments)
        : DeviceObject { render_pass.device() }, m_render_pass { &render_pass },
          m_extent { std::move(extent) }, m_attachments { std::move(attachments) } {
        const auto vk_attachments =
            m_attachments |
            std::views::transform([](auto&& attachment) { return *attachment.vkHandle(); }) |
            std::ranges::to<std::vector>();

        const auto create_info = vk::FramebufferCreateInfo {}
                                     .setRenderPass(*m_render_pass, )
                                     .setAttachments(vk_attachments)
                                     .setWidth(m_extent.width)
                                     .setHeight(m_extent.height)
                                     .setLayers(1);

        try {
            m_vk_framebuffer = vk::raii::Framebuffer { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::~FrameBuffer() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameBuffer::operator=(FrameBuffer&& other) noexcept -> FrameBuffer& = default;
} // namespace stormkit::gpu
