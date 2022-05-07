// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Framebuffer: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Framebuffer;

        Framebuffer(const RenderPass &render_pass,
                    core::Extentu size,
                    TextureViewConstPtrArray attachments);
        ~Framebuffer();

        Framebuffer(Framebuffer &&);
        Framebuffer &operator=(Framebuffer &&);

        inline const core::Extentu &extent() const noexcept;
        inline const Device &device() const noexcept;
        inline storm::core::span<const TextureViewConstPtr> attachments() const noexcept;

        inline vk::Framebuffer vkFramebuffer() const noexcept;
        inline operator vk::Framebuffer() const noexcept;
        inline vk::Framebuffer vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        RenderPassConstPtr m_render_pass;

        core::Extentu m_extent;
        TextureViewConstPtrArray m_attachments;

        RAIIVkFramebuffer m_vk_framebuffer;
    };
} // namespace storm::render

#include "Framebuffer.inl"
