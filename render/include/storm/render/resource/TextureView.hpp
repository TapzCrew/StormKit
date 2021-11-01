// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/TextureSubresourceRange.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC TextureView: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Image_View;

        TextureView(const Device &device,
                    const Texture &texture,
                    TextureViewType type                      = TextureViewType::T2D,
                    TextureSubresourceRange subresource_range = {});
        ~TextureView();

        TextureView(TextureView &&);
        TextureView &operator=(TextureView &&);

        inline const Texture &texture() const noexcept;
        inline TextureViewType type() const noexcept;
        inline const TextureSubresourceRange &subresourceRange() const noexcept;

        inline vk::ImageView vkImageView() const noexcept;
        inline operator vk::ImageView() const noexcept;
        inline vk::ImageView vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstPtr m_device;
        TextureConstPtr m_texture;

        TextureViewType m_type;
        TextureSubresourceRange m_subresource_range;

        RAIIVkImageView m_vk_image_view;
    };
} // namespace storm::render

#include "TextureView.inl"
