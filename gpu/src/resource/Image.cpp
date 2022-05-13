// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/Coroutines.mpp>

/////////// - StormKit::render - ///////////
#include <stormkit/gpu/core/CommandBuffer.mpp>
#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/Queue.mpp>

#include <stormkit/gpu/resource/Buffer.mpp>
#include <stormkit/gpu/resource/Image.mpp>

#include <stormkit/gpu/sync/Fence.mpp>

#include <stormkit/image/Image.mpp>

#include "ConvertFormatShaders.hpp"

//#include <gli/load.hpp>

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
    Image::Image(const Device &device, const CreateInfo &info, Tag)
        : m_device { &device }, m_extent { info.extent }, m_format { info.format },
          m_layers { info.layers }, m_mip_levels { info.mip_levels }, m_type { info.type },
          m_flags { info.flags }, m_samples { info.samples }, m_usage { info.usage } {
        m_faces = 1;
        if (core::checkFlag(m_flags, gpu::ImageCreateFlag::Cube_Compatible)) m_faces = 6u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const Device &device, const CreateInfo &info) : Image { device, info, Tag {} } {
        const auto &vk = m_device->table();

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
            .usage         = core::as<VkImageUsageFlags>(m_usage),
            .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        CHECK_VK_ERROR(vk.vkCreateImage(*m_device, &create_info, nullptr, &m_image));

        const auto requirements = [&] {
            auto r = VkMemoryRequirements {};

            vk.vkGetImageMemoryRequirements(*m_device, m_image, &r);

            return r;
        }();

        const auto allocate_info =
            VmaAllocationCreateInfo { .requiredFlags = core::as<core::UInt32>(info.property) };

        CHECK_VK_ERROR(vmaAllocateMemory(m_device->vmaAllocator(),
                                         &requirements,
                                         &allocate_info,
                                         &m_image_memory,
                                         nullptr));

        CHECK_VK_ERROR(vmaBindImageMemory(m_device->vmaAllocator(), m_image_memory, m_image));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const Device &device,
                 const core::ExtentU &extent,
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
                const auto &vk = device().table();

                vk.vkDestroyImage(device(), m_image, nullptr);
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(Image &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_extent { std::exchange(other.m_extent, { 0, 0, 0 }) },
          m_format { std::exchange(other.m_format, {}) },
          m_layers { std::exchange(other.m_layers, 0) }, m_faces { std::exchange(other.m_faces,
                                                                                 0) },
          m_mip_levels { std::exchange(other.m_mip_levels, 0) },
          m_type { std::exchange(other.m_type, {}) }, m_flags { std::exchange(other.m_flags, {}) },
          m_samples { std::exchange(other.m_samples, {}) }, m_usage { std::exchange(other.m_usage,
                                                                                    {}) },
          m_image_memory { std::exchange(other.m_image_memory, VK_NULL_HANDLE) },
          m_image { std::exchange(other.m_image, VK_NULL_HANDLE) }, m_own_image {
              std::exchange(other.m_own_image, true)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::operator=(Image &&other) noexcept -> Image & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device       = std::exchange(other.m_device, nullptr);
        m_extent       = std::exchange(other.m_extent, { 0, 0, 0 });
        m_format       = std::exchange(other.m_format, {});
        m_layers       = std::exchange(other.m_layers, 0);
        m_faces        = std::exchange(other.m_faces, 0);
        m_mip_levels   = std::exchange(other.m_mip_levels, 0);
        m_type         = std::exchange(other.m_type, {});
        m_flags        = std::exchange(other.m_flags, {});
        m_samples      = std::exchange(other.m_samples, {});
        m_usage        = std::exchange(other.m_usage, {});
        m_image_memory = std::exchange(other.m_image_memory, VK_NULL_HANDLE);
        m_image        = std::exchange(other.m_image, VK_NULL_HANDLE);
        m_own_image    = std::exchange(other.m_own_image, true);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromImage(const image::Image &image, bool generate_mips) -> void {
        auto staging_buffer = m_device->createStagingBuffer(image.size());

        auto fence = m_device->createFence();
        auto command_buffer =
            m_device->graphicsQueue().createCommandBuffer(CommandBufferLevel::Primary);

        command_buffer.begin(true);

        loadFromImage(image, command_buffer, staging_buffer, 0u, generate_mips);

        command_buffer.end();
        command_buffer.submit({}, {}, &fence);

        fence.wait();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromImage(const image::Image &image,
                              CommandBuffer &command_buffer,
                              Buffer &staging_buffer,
                              core::UInt32 offset,
                              bool generate_mips) -> void {
        STORMKIT_EXPECTS(m_own_image);

        loadFromMemory(image.data(),
                       image.layers(),
                       image.faces(),
                       image.mipLevels(),
                       command_buffer,
                       staging_buffer,
                       offset,
                       generate_mips);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromMemory(core::ByteConstSpan data,
                               core::UInt32 layers,
                               core::UInt32 faces,
                               core::UInt32 mip_levels,
                               bool generate_mips) -> void {
        auto staging_buffer = m_device->createStagingBuffer(std::size(data));

        auto fence = m_device->createFence();
        auto command_buffer =
            m_device->graphicsQueue().createCommandBuffer(CommandBufferLevel::Primary);

        command_buffer.begin(true);

        loadFromMemory(data,
                       layers,
                       faces,
                       mip_levels,
                       command_buffer,
                       staging_buffer,
                       0u,
                       generate_mips);

        command_buffer.end();
        command_buffer.submit({}, {}, core::makeObserver(fence));

        fence.wait();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromMemory(core::ByteConstSpan data,
                               core::UInt32 layers,
                               core::UInt32 faces,
                               core::UInt32 mip_levels,
                               gpu::CommandBuffer &command_buffer,
                               gpu::Buffer &buffer,
                               core::UInt32 offset,
                               bool generate_mips) -> void {
        buffer.upload(data);

        command_buffer.beginDebugRegion("Upload Image", core::RGBColorDef::Lime<float>);
        command_buffer.transitionImageLayout(*this,
                                             ImageLayout::Undefined,
                                             ImageLayout::Transfer_Dst_Optimal,
                                             { .level_count = m_mip_levels,
                                               .layer_count = m_layers * m_faces });

        const auto channel_count         = getChannelCountFor(m_format);
        const auto byte_count_by_channel = getArraySizeByChannelFor(m_format);

        auto _offset = 0u;
        for (auto [layer, face, mip_level] : core::generateIndices(layers, faces, mip_levels)) {
            const auto extent = core::ExtentU { std::max(1u, m_extent.width >> mip_level),
                                                std::max(1u, m_extent.height >> mip_level),
                                                std::max(1u, m_extent.depth >> mip_level) };

            const auto size =
                extent.width * extent.height * extent.depth * channel_count * byte_count_by_channel;

            auto copy_regions = std::vector<BufferImageCopy> {};
            copy_regions.reserve(m_layers);

            copy_regions.emplace_back(BufferImageCopy {
                .buffer_offset      = offset + _offset,
                .subresource_layers = { .mip_level = core::as<core::UInt32>(mip_level),
                                        .base_array_layer =
                                            core::as<core::UInt32>(layer + (face * m_layers)) },
                .extent             = extent,
            });

            command_buffer.copyBufferToImage(buffer, *this, std::move(copy_regions));

            _offset += size;
        }

        auto layout = ImageLayout::Transfer_Dst_Optimal;
        if (generate_mips) {
            generateMipmap(command_buffer, m_mip_levels);

            layout = ImageLayout::Transfer_Src_Optimal;
        }

        command_buffer.transitionImageLayout(*this,
                                             layout,
                                             ImageLayout::Shader_Read_Only_Optimal,
                                             { .level_count = m_mip_levels,
                                               .layer_count = m_layers * m_faces });
        command_buffer.endDebugRegion();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::createView(ImageViewType type,
                           ImageSubresourceRange subresource_range) const noexcept -> ImageView {
        return ImageView { *m_device, *this, type, subresource_range };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::allocateView(ImageViewType type, ImageSubresourceRange subresource_range) const
        -> ImageViewOwnedPtr {
        return std::make_unique<ImageView>(*m_device, *this, type, subresource_range);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::allocateRefCountedView(ImageViewType type,
                                       ImageSubresourceRange subresource_range) const
        -> ImageViewSharedPtr {
        return std::make_shared<ImageView>(*m_device, *this, type, subresource_range);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::generateMipmap(CommandBuffer &command_buffer, core::UInt32 mip_levels) -> void {
        command_buffer.beginDebugRegion("Generate Image mips", core::RGBColorDef::Maroon<float>);
        command_buffer.transitionImageLayout(*this,
                                             ImageLayout::Transfer_Dst_Optimal,
                                             ImageLayout::Transfer_Src_Optimal,
                                             { .layer_count = m_layers * m_faces });

        for (auto i : core::range(1u, mip_levels)) {
            auto region =
                BlitRegion { .source = ImageSubresourceLayers { .mip_level   = i - 1u,
                                                                .layer_count = m_layers * m_faces },
                             .destination =
                                 ImageSubresourceLayers { .mip_level   = i,
                                                          .layer_count = m_layers * m_faces },
                             .source_offset      = { core::ExtentU {},
                                                core::ExtentU {
                                                    std::max(1u, m_extent.width >> (i - 1u)),
                                                    std::max(1u, m_extent.height >> (i - 1u)),
                                                    std::max(1u, m_extent.depth >> (i - 1u)),
                                                } },
                             .destination_offset = { core::ExtentU {},
                                                     core::ExtentU {
                                                         std::max(1u, m_extent.width >> (i)),
                                                         std::max(1u, m_extent.height >> (i)),
                                                         std::max(1u, m_extent.depth >> (i)),
                                                     } } };

            command_buffer.blitImage(*this,
                                     *this,
                                     gpu::ImageLayout::Transfer_Src_Optimal,
                                     gpu::ImageLayout::Transfer_Dst_Optimal,
                                     { &region, 1 },
                                     gpu::Filter::Linear);

            command_buffer.transitionImageLayout(*this,
                                                 ImageLayout::Transfer_Dst_Optimal,
                                                 ImageLayout::Transfer_Src_Optimal,
                                                 { .base_mip_level = i,
                                                   .layer_count    = m_layers * m_faces });
        }
        command_buffer.endDebugRegion();
    }
} // namespace stormkit::gpu
