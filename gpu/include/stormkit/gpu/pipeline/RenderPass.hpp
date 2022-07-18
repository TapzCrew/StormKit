// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>
#include <stormkit/gpu/pipeline/RenderPassDescription.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC RenderPass: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Render_Pass;

        // RenderPass(const Device &device, vk::RenderPass render_pass);
        RenderPass(const Device &device, const RenderPassDescription &description);
        ~RenderPass();

        RenderPass(const RenderPass &) = delete;
        auto operator=(const RenderPass &) -> RenderPass & = delete;

        RenderPass(RenderPass &&) noexcept;
        auto operator=(RenderPass &&) noexcept -> RenderPass &;

        [[nodiscard]] auto createFramebuffer(const core::ExtentU &extent,
                                             std::vector<ImageViewConstRef> attachments) const
            -> Framebuffer;
        [[nodiscard]] auto allocateFramebuffer(const core::ExtentU &extent,
                                               std::vector<ImageViewConstRef> attachments) const
            -> FramebufferOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedFramebuffer(const core::ExtentU &extent,
                                          std::vector<ImageViewConstRef> attachments) const
            -> FramebufferSharedPtr;

        [[nodiscard]] auto isCompatible(const RenderPass &render_pass) const noexcept -> bool;

        [[nodiscard]] auto description() const noexcept -> const RenderPassDescription &;

        [[nodiscard]] operator VkRenderPass() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkRenderPass;

      private:
        auto bake() noexcept -> void;

        RenderPassDescription m_description = {};

        VkRenderPass m_render_pass = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "RenderPass.inl"
