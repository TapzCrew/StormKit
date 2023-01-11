// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Resource;

import :Image;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

// #include <gli/load.hpp>

namespace stormkit::gpu {
    /* constexpr auto toPixelFormat(gli::format format) {
    switch (format) {
        case gli::FORMAT_RGBA16_SFLOAT_PACK16: return gpu::PixelFormat::RGBA16F;
        default: STORMKIT_ENSURES(false);
    }*/

    constexpr auto toPixelFormat(image::Image::Format format) noexcept {
        switch (format) {
            case image::Image::Format::R8_SNorm: return PixelFormat::R8_SNorm;
            case image::Image::Format::R8_UNorm: return PixelFormat::R8_UNorm;
            case image::Image::Format::RG8_SNorm: return PixelFormat::RG8_SNorm;
            case image::Image::Format::RG8_UNorm: return PixelFormat::RG8_UNorm;
            case image::Image::Format::R8I: return PixelFormat::R8I;
            case image::Image::Format::R8U: return PixelFormat::R8U;
            case image::Image::Format::RG8I: return PixelFormat::RG8I;
            case image::Image::Format::RG8U: return PixelFormat::RG8U;
            case image::Image::Format::RGB8_SNorm: return PixelFormat::RGB8_SNorm;
            case image::Image::Format::RGB8_UNorm: return PixelFormat::RGB8_UNorm;
            case image::Image::Format::BGR8_UNorm: return PixelFormat::BGR8_UNorm;
            case image::Image::Format::RGB8I: return PixelFormat::RGB8I;
            case image::Image::Format::RGB8U: return PixelFormat::RGB8U;
            case image::Image::Format::RGBA8_SNorm: return PixelFormat::RGBA8_SNorm;
            case image::Image::Format::RGBA8_UNorm: return PixelFormat::RGBA8_UNorm;
            case image::Image::Format::RGBA16_SNorm: return PixelFormat::RGBA16_SNorm;
            case image::Image::Format::BGRA8_UNorm: return PixelFormat::BGRA8_UNorm;
            case image::Image::Format::sRGB8: return PixelFormat::sRGB8;
            case image::Image::Format::sBGR8: return PixelFormat::sBGR8;
            case image::Image::Format::sRGBA8: return PixelFormat::sRGBA8;
            case image::Image::Format::sBGRA8: return PixelFormat::sBGRA8;

            case image::Image::Format::R16_SNorm: return PixelFormat::R16_SNorm;
            case image::Image::Format::R16_UNorm: return PixelFormat::R16_UNorm;
            case image::Image::Format::R16I: return PixelFormat::R16I;
            case image::Image::Format::R16U: return PixelFormat::R16U;
            case image::Image::Format::RG16_SNorm: return PixelFormat::RG16_SNorm;
            case image::Image::Format::RG16_UNorm: return PixelFormat::RG16_UNorm;
            case image::Image::Format::RG16I: return PixelFormat::RG16I;
            case image::Image::Format::RG16U: return PixelFormat::RG16U;
            case image::Image::Format::RG16F: return PixelFormat::RG16F;
            case image::Image::Format::RGB16I: return PixelFormat::RGB16I;
            case image::Image::Format::RGB16U: return PixelFormat::RGB16U;
            case image::Image::Format::RGB16F: return PixelFormat::RGB16F;
            case image::Image::Format::RGBA16I: return PixelFormat::RGBA16I;
            case image::Image::Format::RGBA16U: return PixelFormat::RGBA16U;
            case image::Image::Format::RGBA16F: return PixelFormat::RGBA16F;
            case image::Image::Format::R16F: return PixelFormat::R16F;

            case image::Image::Format::R32I: return PixelFormat::R32I;
            case image::Image::Format::R32U: return PixelFormat::R32U;
            case image::Image::Format::R32F: return PixelFormat::R32F;
            case image::Image::Format::RG32I: return PixelFormat::RG32I;
            case image::Image::Format::RG32U: return PixelFormat::RG32U;
            case image::Image::Format::RG32F: return PixelFormat::RG32F;
            case image::Image::Format::RGB16_SNorm: return PixelFormat::RGB16_SNorm;
            case image::Image::Format::RGB32I: return PixelFormat::RGB32I;
            case image::Image::Format::RGB32U: return PixelFormat::RGB32U;
            case image::Image::Format::RGB32F: return PixelFormat::RGB32F;
            case image::Image::Format::RGBA8I: return PixelFormat::RGBA8I;
            case image::Image::Format::RGBA8U: return PixelFormat::RGBA8U;
            case image::Image::Format::RGBA32I: return PixelFormat::RGBA32I;
            case image::Image::Format::RGBA32U: return PixelFormat::RGBA32U;
            case image::Image::Format::RGBA32F: return PixelFormat::RGBA32F;

            default: return PixelFormat::Undefined;
        }

        return PixelFormat::Undefined;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const Device& device, const CreateInfo& info, Tag)
        : DeviceObject { device }, m_extent { info.extent }, m_format { info.format },
          m_layers { info.layers }, m_faces { 1 }, m_mip_levels { info.mip_levels },
          m_type { info.type }, m_flags { info.flags }, m_samples { info.samples },
          m_usages { info.usages } {
        if (core::checkFlag(m_flags, gpu::ImageCreateFlag::Cube_Compatible)) m_faces = 6u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const Device& device, const CreateInfo& info) : Image { device, info, Tag {} } {
        const auto& vk = this->device().table();

        const auto create_info = VkImageCreateInfo {
            .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags         = core::as<VkImageCreateFlags>(m_flags),
            .imageType     = core::as<VkImageType>(m_type),
            .format        = core::as<VkFormat>(m_format),
            .extent        = VkExtent3D { .width  = m_extent.width,
                                          .height = m_extent.height,
                                          .depth  = m_extent.depth },
            .mipLevels     = m_mip_levels,
            .arrayLayers   = m_layers * m_faces,
            .samples       = core::as<VkSampleCountFlagBits>(m_samples),
            .tiling        = core::as<VkImageTiling>(info.tiling),
            .usage         = core::as<VkImageUsageFlags>(m_usages),
            .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        CHECK_VK_ERROR(vk.vkCreateImage(this->device(), &create_info, nullptr, &m_image));

        const auto requirements = [&] {
            auto r = VkMemoryRequirements {};

            vk.vkGetImageMemoryRequirements(this->device(), m_image, &r);

            return r;
        }();

        const auto allocate_info =
            VmaAllocationCreateInfo { .requiredFlags = core::as<core::UInt32>(info.property) };

        CHECK_VK_ERROR(vmaAllocateMemory(this->device().vmaAllocator(),
                                         &requirements,
                                         &allocate_info,
                                         &m_image_memory,
                                         nullptr));

        CHECK_VK_ERROR(vmaBindImageMemory(this->device().vmaAllocator(), m_image_memory, m_image));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const Device& device,
                 const core::math::ExtentU& extent,
                 gpu::PixelFormat format,
                 VkImage image)
        : Image { device, CreateInfo { extent, format, 1u, 1u }, Tag {} } {
        m_image     = image;
        m_own_image = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::~Image() {
        if (m_own_image) [[likely]] {
            if (m_image_memory != VK_NULL_HANDLE) [[likely]]
                vmaFreeMemory(device().vmaAllocator(), m_image_memory);

            if (m_image != VK_NULL_HANDLE) [[likely]] {
                const auto& vk = device().table();

                vk.vkDestroyImage(device(), m_image, nullptr);
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(Image&& other) noexcept
        : DeviceObject { std::move(other) },
          m_extent { std::exchange(other.m_extent, { 0, 0, 0 }) },
          m_format { std::exchange(other.m_format, {}) },
          m_layers { std::exchange(other.m_layers, 0) },
          m_faces { std::exchange(other.m_faces, 0) },
          m_mip_levels { std::exchange(other.m_mip_levels, 0) },
          m_type { std::exchange(other.m_type, {}) }, m_flags { std::exchange(other.m_flags, {}) },
          m_samples { std::exchange(other.m_samples, {}) },
          m_usages { std::exchange(other.m_usages, {}) },
          m_image { std::exchange(other.m_image, VK_NULL_HANDLE) },
          m_image_memory { std::exchange(other.m_image_memory, VK_NULL_HANDLE) },
          m_own_image { std::exchange(other.m_own_image, true) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::operator=(Image&& other) noexcept -> Image& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_extent       = std::exchange(other.m_extent, { 0, 0, 0 });
        m_format       = std::exchange(other.m_format, {});
        m_layers       = std::exchange(other.m_layers, 0);
        m_faces        = std::exchange(other.m_faces, 0);
        m_mip_levels   = std::exchange(other.m_mip_levels, 0);
        m_type         = std::exchange(other.m_type, {});
        m_flags        = std::exchange(other.m_flags, {});
        m_samples      = std::exchange(other.m_samples, {});
        m_usages       = std::exchange(other.m_usages, {});
        m_image_memory = std::exchange(other.m_image_memory, VK_NULL_HANDLE);
        m_image        = std::exchange(other.m_image, VK_NULL_HANDLE);
        m_own_image    = std::exchange(other.m_own_image, true);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::createView(ImageViewType type,
                           ImageSubresourceRange subresource_range) const noexcept -> ImageView {
        return ImageView { device(), *this, type, subresource_range };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::allocateView(ImageViewType type, ImageSubresourceRange subresource_range) const
        -> std::unique_ptr<ImageView> {
        return std::make_unique<ImageView>(device(), *this, type, subresource_range);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::allocateRefCountedView(ImageViewType type,
                                       ImageSubresourceRange subresource_range) const
        -> std::shared_ptr<ImageView> {
        return std::make_shared<ImageView>(device(), *this, type, subresource_range);
    }
} // namespace stormkit::gpu
