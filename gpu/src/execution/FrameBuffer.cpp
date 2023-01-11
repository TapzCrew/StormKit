// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :FrameBuffer;
import :RenderPass;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::FrameBuffer(const RenderPass& render_pass,
                             const core::math::ExtentU& extent,
                             std::vector<ImageViewConstRef> attachments)
        : DeviceObject { render_pass.device() }, m_render_pass { &render_pass },
          m_extent { std::move(extent) }, m_attachments { std::move(attachments) } {
        const auto& vk = this->device().table();

        const auto _attachments = [&] {
            auto v = std::vector<VkImageView> {};
            std::ranges::transform(m_attachments,
                                   std::back_inserter(v),
                                   [](const auto& a) -> VkImageView { return a.get(); });

            return v;
        }();

        const auto create_info =
            VkFramebufferCreateInfo { .sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                      .pNext      = nullptr,
                                      .flags      = {},
                                      .renderPass = *m_render_pass,
                                      .attachmentCount =
                                          core::as<core::UInt32>(std::size(_attachments)),
                                      .pAttachments = std::data(_attachments),
                                      .width        = m_extent.width,
                                      .height       = m_extent.height,
                                      .layers       = 1 };

        CHECK_VK_ERROR(
            vk.vkCreateFramebuffer(this->device(), &create_info, nullptr, &m_framebuffer));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::~FrameBuffer() {
        if (m_framebuffer != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyFramebuffer(device(), m_framebuffer, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
        : DeviceObject { std::move(other) },
          m_render_pass { std::exchange(other.m_render_pass, nullptr) },
          m_extent { std::exchange(other.m_extent, { 0, 0 }) },
          m_attachments { std::exchange(other.m_attachments, {}) },
          m_framebuffer { std::exchange(other.m_framebuffer, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameBuffer::operator=(FrameBuffer&& other) noexcept -> FrameBuffer& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_render_pass = std::exchange(other.m_render_pass, nullptr);
        m_extent      = std::exchange(other.m_extent, { 0, 0 });
        m_attachments = std::exchange(other.m_attachments, {});
        m_framebuffer = std::exchange(other.m_framebuffer, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
