// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/gpu/core/Device.mpp>

#include <stormkit/gpu/resource/Image.mpp>
#include <stormkit/gpu/resource/ImageView.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::ImageView(const Device &device,
                         const Image &image,
                         ImageViewType type,
                         ImageSubresourceRange subresource_range)
        : m_device { &device }, m_image { &image }, m_type { type }, m_subresource_range {
              subresource_range
          } {
        const auto &vk = m_device->table();

        const auto vk_subresource_range = VkImageSubresourceRange {
            .aspectMask     = core::as<VkImageAspectFlags>(m_subresource_range.aspect_mask),
            .baseMipLevel   = m_subresource_range.base_mip_level,
            .levelCount     = m_subresource_range.level_count,
            .baseArrayLayer = m_subresource_range.base_array_layer,
            .layerCount     = m_subresource_range.layer_count,
        };

        const auto components = VkComponentMapping {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A,
        };

        const auto create_info = VkImageViewCreateInfo {
            .sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image            = *m_image,
            .viewType         = core::as<VkImageViewType>(m_type),
            .format           = core::as<VkFormat>(m_image->format()),
            .components       = components,
            .subresourceRange = vk_subresource_range,
        };

        CHECK_VK_ERROR(vk.vkCreateImageView(*m_device, &create_info, nullptr, &m_image_view));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::~ImageView() {
        if (m_image_view != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyImageView(device(), m_image_view, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    ImageView::ImageView(ImageView &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_image { std::exchange(other.m_image, nullptr) }, m_type { std::exchange(other.m_type,
                                                                                    {}) },
          m_subresource_range { std::exchange(other.m_subresource_range, {}) }, m_image_view {
              std::exchange(other.m_image_view, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto ImageView::operator=(ImageView &&other) noexcept -> ImageView & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device            = std::exchange(other.m_device, nullptr);
        m_image             = std::exchange(other.m_image, nullptr);
        m_type              = std::exchange(other.m_type, {});
        m_subresource_range = std::exchange(other.m_subresource_range, {});
        m_image_view        = std::exchange(other.m_image_view, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
