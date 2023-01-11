// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Core;

import :Resource.Image;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::ImageView(const Device& device,
                         const Image& image,
                         ImageViewType type,
                         ImageSubresourceRange subresource_range)
        : DeviceObject { device }, ImageObject { image }, m_type { type },
          m_subresource_range { subresource_range } {
        const auto vk_subresource_range =
            vk::ImageSubresourceRange {}
                .setAspectMask(core::as<vk::ImageAspectFlagBits>(m_subresource_range.aspect_mask))
                .setBaseMipLevel(m_subresource_range.base_mip_level)
                .setLevelCount(m_subresource_range.level_count)
                .setBaseArrayLayer(m_subresource_range.base_array_layer)
                .setLayerCount(m_subresource_range.layer_count);

        const auto components = vk::ComponentMapping {}
                                    .setR(vk::ComponentSwizzle::eR)
                                    .setG(vk::ComponentSwizzle::eG)
                                    .setB(vk::ComponentSwizzle::eB)
                                    .setA(vk::ComponentSwizzle::eA);

        const auto create_info = vk::ImageViewCreateInfo {}
                                     .setImage(*this->image().vkHandle())
                                     .setViewType(core::as<vk::ImageViewType>(m_type))
                                     .setFormat(core::as<vk::Format>(this->image().format()))
                                     .setComponents(components)
                                     .setSubresourceRange(vk_subresource_range);

        try {
            m_vk_image_view = vk::raii::ImageView { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::~ImageView() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::ImageView(ImageView&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto ImageView::operator=(ImageView&& other) noexcept -> ImageView& = default;
} // namespace stormkit::gpu
