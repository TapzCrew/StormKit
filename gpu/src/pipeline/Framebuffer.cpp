// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.hpp>

#include <stormkit/gpu/pipeline/Framebuffer.hpp>
#include <stormkit/gpu/pipeline/RenderPass.hpp>

#include <stormkit/gpu/resource/Image.hpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Framebuffer::Framebuffer(const RenderPass& render_pass,
                             const core::ExtentU& extent,
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
    Framebuffer::~Framebuffer() {
        if (m_framebuffer != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyFramebuffer(device(), m_framebuffer, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
        : DeviceObject { std::move(other) }, m_render_pass { std::exchange(other.m_render_pass,
                                                                           nullptr) },
          m_extent { std::exchange(other.m_extent, { 0, 0 }) },
          m_attachments { std::exchange(other.m_attachments, {}) }, m_framebuffer {
              std::exchange(other.m_framebuffer, VK_NULL_HANDLE)
          } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Framebuffer::operator=(Framebuffer&& other) noexcept -> Framebuffer& {
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
