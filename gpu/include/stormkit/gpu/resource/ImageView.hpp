// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/ImageSubresourceRange.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC ImageView: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Image_View;

        ImageView(const Device &device,
                  const Image &image,
                  ImageViewType type                      = ImageViewType::T2D,
                  ImageSubresourceRange subresource_range = {});
        ~ImageView();

        ImageView(const ImageView &) = delete;
        auto operator=(const ImageView &) -> ImageView & = delete;

        ImageView(ImageView &&) noexcept;
        auto operator=(ImageView &&) noexcept -> ImageView &;

        [[nodiscard]] auto image() const noexcept -> const Image &;
        [[nodiscard]] auto type() const noexcept -> ImageViewType;
        [[nodiscard]] auto subresourceRange() const noexcept -> const ImageSubresourceRange &;

        [[nodiscard]] operator VkImageView() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkImageView;

      private:
        const Image *m_image = nullptr;

        ImageViewType m_type                      = {};
        ImageSubresourceRange m_subresource_range = {};

        VkImageView m_image_view = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "ImageView.inl"