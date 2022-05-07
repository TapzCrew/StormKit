// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::core - ///////////
#include <storm/core/Span.hpp>
#include <storm/core/Format.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>

#include "ConvertFormatShaders.hpp"

#include <gli/load.hpp>

using namespace storm;
using namespace storm::render;

#define DELETER [](auto handle, const auto &device) { device->deallocateVmaAllocation(handle); }

constexpr auto toPixelFormat(gli::format format) {
    switch (format) {
        case gli::FORMAT_RGBA16_SFLOAT_PACK16: return render::PixelFormat::RGBA16F;
        default: STORMKIT_ENSURES(false);
    }
}

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
Texture::Texture(const Device &device,
                 core::Extentu extent,
                 PixelFormat format,
                 core::UInt32 layers,
                 core::UInt32 mip_levels,
                 TextureType type,
                 TextureCreateFlag flags,
                 SampleCountFlag samples,
                 TextureUsage usage,
                 TextureTiling tiling,
                 MemoryProperty property)
    : m_device { &device }, m_extent { std::move(extent) }, m_format { format },
      m_layers { layers }, m_mip_levels { mip_levels }, m_type { type }, m_flags { flags },
      m_samples { samples }, m_usage { usage }, m_vma_texture_memory { DELETER, *m_device } {
    if (core::checkFlag(m_flags, render::TextureCreateFlag::Cube_Compatible)) m_faces = 6u;

    const auto create_info = vk::ImageCreateInfo {}
                                 .setImageType(toVK(m_type))
                                 .setFormat(toVK(m_format))
                                 .setExtent(VkExtent3D { .width  = m_extent.width,
                                                         .height = m_extent.height,
                                                         .depth  = m_extent.depth })
                                 .setMipLevels(m_mip_levels)
                                 .setArrayLayers(m_layers * m_faces)
                                 .setSamples(toVKBits(m_samples))
                                 .setTiling(toVK(tiling))
                                 .setUsage(toVK(usage))
                                 .setSharingMode(vk::SharingMode::eExclusive)
                                 .setInitialLayout(vk::ImageLayout::eUndefined)
                                 .setFlags(toVK(m_flags));

    m_vk_texture = m_device->createVkImage(create_info);

    const auto requirements = m_device->getVkImageMemoryRequirements(*m_vk_texture);
    const auto alloc_info =
        VmaAllocationCreateInfo { .requiredFlags = toVK(property).operator unsigned int() };

    m_vma_texture_memory.reset(m_device->allocateVmaAllocation(alloc_info, requirements));

    m_device->bindVmaImageMemory(m_vma_texture_memory, *m_vk_texture);
}

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(const Device &device,
                 core::Extentu extent,
                 render::PixelFormat format,
                 vk::Image image)
    : Texture { device, std::move(extent), format, 1u, 1u } {
    m_non_owning_texture = std::move(image);
}

/////////////////////////////////////
/////////////////////////////////////
Texture::~Texture() = default;

/////////////////////////////////////
/////////////////////////////////////
Texture::Texture(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Texture &Texture::operator=(Texture &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(const image::Image &image, bool generate_mips) {
    auto staging_buffer = m_device->createStagingBuffer(image.size());

    auto fence = m_device->createFence();
    auto command_buffer =
        m_device->graphicsQueue().createCommandBuffer(CommandBufferLevel::Primary);

    command_buffer.begin(true);

    loadFromImage(image, command_buffer, staging_buffer, 0u, generate_mips);

    command_buffer.end();
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromImage(const image::Image &image,
                            render::CommandBuffer &command_buffer,
                            render::HardwareBuffer &staging_buffer,
                            core::UInt32 offset,
                            bool generate_mips) {
    STORMKIT_EXPECTS(!m_non_owning_texture);

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
void Texture::loadFromMemory(core::ByteConstSpan data,
                             core::UInt32 layers,
                             core::UInt32 faces,
                             core::UInt32 mip_levels,
                             bool generate_mips) {
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
    command_buffer.build();
    command_buffer.submit({}, {}, core::makeObserver(fence));

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::loadFromMemory(core::ByteConstSpan data,
                             core::UInt32 layers,
                             core::UInt32 faces,
                             core::UInt32 mip_levels,
                             render::CommandBuffer &command_buffer,
                             render::HardwareBuffer &buffer,
                             core::UInt32 offset,
                             bool generate_mips) {
    buffer.upload<core::Byte>(data);

    command_buffer.beginDebugRegion("Upload Texture", core::RGBColorDef::Lime<float>);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Undefined,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           { .level_count = m_mip_levels,
                                             .layer_count = m_layers * m_faces });

    const auto channel_count         = getChannelCountFor(m_format);
    const auto byte_count_by_channel = getArraySizeByChannelFor(m_format);

    auto _offset = 0u;
    for (auto layer = 0u; layer < layers; ++layer) {
        for (auto face = 0u; face < faces; ++face) {
            for (auto mip_level = 0u; mip_level < mip_levels; ++mip_level) {
                const auto extent = core::Extentu { std::max(1u, m_extent.width >> mip_level),
                                                    std::max(1u, m_extent.height >> mip_level),
                                                    std::max(1u, m_extent.depth >> mip_level) };

                const auto size = extent.width * extent.height * extent.depth * channel_count *
                                  byte_count_by_channel;

                auto copy_regions = std::vector<BufferTextureCopy> {};
                copy_regions.reserve(m_layers);

                copy_regions.emplace_back(BufferTextureCopy {
                    .buffer_offset      = offset + _offset,
                    .subresource_layers = { .mip_level        = mip_level,
                                            .base_array_layer = layer + (face * m_layers) },
                    .extent             = extent,
                });

                command_buffer.copyBufferToTexture(buffer, *this, std::move(copy_regions));

                _offset += size;
            }
        }
    }

    auto layout = TextureLayout::Transfer_Dst_Optimal;
    if (generate_mips) {
        generateMipmap(command_buffer, m_mip_levels);

        layout = TextureLayout::Transfer_Src_Optimal;
    }

    command_buffer.transitionTextureLayout(*this,
                                           layout,
                                           TextureLayout::Shader_Read_Only_Optimal,
                                           { .level_count = m_mip_levels,
                                             .layer_count = m_layers * m_faces });
    command_buffer.endDebugRegion();
}

/////////////////////////////////////
/////////////////////////////////////
TextureView Texture::createView(TextureViewType type,
                                TextureSubresourceRange subresource_range) const noexcept {
    return TextureView { *m_device, *this, type, std::move(subresource_range) };
}

/////////////////////////////////////
/////////////////////////////////////
TextureViewOwnedPtr Texture::createViewPtr(TextureViewType type,
                                           TextureSubresourceRange subresource_range) const {
    return std::make_unique<TextureView>(*m_device, *this, type, std::move(subresource_range));
}

/////////////////////////////////////
/////////////////////////////////////
void Texture::generateMipmap(render::CommandBuffer &command_buffer, core::UInt32 mip_levels) {
    command_buffer.beginDebugRegion("Generate Texture mips", core::RGBColorDef::Maroon<float>);
    command_buffer.transitionTextureLayout(*this,
                                           TextureLayout::Transfer_Dst_Optimal,
                                           TextureLayout::Transfer_Src_Optimal,
                                           { .layer_count = m_layers * m_faces });

    for (auto i = 1u; i < mip_levels; ++i) {
        auto region = render::BlitRegion {
            .source             = render::TextureSubresourceLayers { .mip_level   = i - 1u,
                                                         .layer_count = m_layers * m_faces },
            .destination        = render::TextureSubresourceLayers { .mip_level   = i,
                                                              .layer_count = m_layers * m_faces },
            .source_offset      = { core::ExtentuOffset {},
                               core::ExtentuOffset {
                                   std::max(1u, m_extent.width >> (i - 1u)),
                                   std::max(1u, m_extent.height >> (i - 1u)),
                                   std::max(1u, m_extent.depth >> (i - 1u)),
                               } },
            .destination_offset = { core::ExtentuOffset {},
                                    core::ExtentuOffset {
                                        std::max(1u, m_extent.width >> (i)),
                                        std::max(1u, m_extent.height >> (i)),
                                        std::max(1u, m_extent.depth >> (i)),
                                    } }
        };

        command_buffer.blitTexture(*this,
                                   *this,
                                   render::TextureLayout::Transfer_Src_Optimal,
                                   render::TextureLayout::Transfer_Dst_Optimal,
                                   { std::move(region) },
                                   render::Filter::Linear);

        command_buffer.transitionTextureLayout(*this,
                                               TextureLayout::Transfer_Dst_Optimal,
                                               TextureLayout::Transfer_Src_Optimal,
                                               { .base_mip_level = i,
                                                 .layer_count    = m_layers * m_faces });
    }
    command_buffer.endDebugRegion();
}
