// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Framebuffer: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Framebuffer;

        Framebuffer(const RenderPass &render_pass,
                    const core::ExtentU &size,
                    std::vector<ImageViewConstRef> attachments);
        ~Framebuffer();

        Framebuffer(const Framebuffer &) = delete;
        auto operator=(const Framebuffer &) -> Framebuffer & = delete;

        Framebuffer(Framebuffer &&) noexcept;
        auto operator=(Framebuffer &&) noexcept -> Framebuffer &;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU &;
        [[nodiscard]] auto attachments() const noexcept -> std::span<const ImageViewConstRef>;

        [[nodiscard]] operator VkFramebuffer() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkFramebuffer;

      private:
        const RenderPass *m_render_pass = nullptr;

        core::ExtentU m_extent = { 0, 0 };
        std::vector<ImageViewConstRef> m_attachments;

        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "Framebuffer.inl"