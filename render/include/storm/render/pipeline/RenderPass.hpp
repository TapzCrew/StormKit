// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/RenderPassDescription.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC RenderPass: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Render_Pass;

        RenderPass(const Device &device, vk::RenderPass render_pass);
        explicit RenderPass(const Device &device, RenderPassDescription description);
        ~RenderPass();

        RenderPass(RenderPass &&);
        RenderPass &operator=(RenderPass &&);

        Framebuffer createFramebuffer(core::Extentu extent,
                                      TextureViewConstPtrArray attachments) const;
        FramebufferOwnedPtr createFramebufferPtr(core::Extentu extent,
                                                 TextureViewConstPtrArray attachments) const;

        bool isCompatible(const RenderPass &render_pass) const noexcept;

        inline const Device &device() const noexcept;

        inline const RenderPassDescription &description() const noexcept;

        inline vk::RenderPass vkRenderPass() const noexcept;
        inline operator vk::RenderPass() const noexcept;
        inline vk::RenderPass vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        void build();

        DeviceConstPtr m_device;

        RenderPassDescription m_description;

        std::variant<RAIIVkRenderPass, vk::RenderPass> m_vk_render_pass;
    };
} // namespace storm::render

#include "RenderPass.inl"
