// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/ImageView.hpp>

#include <stormkit/image/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Image: public DeviceObject {
      public:
        struct CreateInfo {
            core::ExtentU extent;
            gpu::PixelFormat format = gpu::PixelFormat::RGBA8_UNorm;
            core::UInt32 layers     = 1u;
            core::UInt32 mip_levels = 1u;
            ImageType type          = ImageType::T2D;
            ImageCreateFlag flags   = ImageCreateFlag::None;
            SampleCountFlag samples = SampleCountFlag::C1;
            ImageUsageFlag usages   = ImageUsageFlag::Sampled | ImageUsageFlag::Transfert_Dst |
                                    ImageUsageFlag::Transfert_Src;
            ImageTiling tiling          = ImageTiling::Optimal;
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local;
        };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Image;

        Image(const Device &device, const CreateInfo &create_info);
        Image(const Device &device,
              const core::ExtentU &extent,
              gpu::PixelFormat format,
              VkImage image);
        ~Image();

        Image(const Image &) = delete;
        auto operator=(const Image &) -> Image & = delete;

        Image(Image &&) noexcept;
        auto operator=(Image &&) noexcept -> Image &;

        auto loadFromImage(const image::Image &image, bool generate_mips = false) -> void;
        auto loadFromImage(const image::Image &image,
                           gpu::CommandBuffer &command_buffer,
                           gpu::Buffer &buffer,
                           core::UInt32 offset = 0u,
                           bool generate_mips  = false) -> void;

        auto loadFromMemory(core::ByteConstSpan data,
                            core::UInt32 layers,
                            core::UInt32 faces,
                            core::UInt32 mip_levels,
                            bool generate_mips = false) -> void;
        auto loadFromMemory(core::ByteConstSpan data,
                            core::UInt32 layers,
                            core::UInt32 faces,
                            core::UInt32 mip_levels,
                            gpu::CommandBuffer &command_buffer,
                            gpu::Buffer &buffer,
                            core::UInt32 offset = 0u,
                            bool generate_mips  = false) -> void;

        [[nodiscard]] auto createView(ImageViewType type                      = ImageViewType::T2D,
                                      ImageSubresourceRange subresource_range = {}) const noexcept
            -> ImageView;
        [[nodiscard]] auto allocateView(ImageViewType type = ImageViewType::T2D,
                                        ImageSubresourceRange subresource_range = {}) const
            -> ImageViewOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedView(ImageViewType type                      = ImageViewType::T2D,
                                   ImageSubresourceRange subresource_range = {}) const
            -> ImageViewSharedPtr;

        auto generateMipmap(CommandBuffer &cmb, core::UInt32 mip_level) -> void;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU &;
        [[nodiscard]] auto format() const noexcept -> PixelFormat;
        [[nodiscard]] auto type() const noexcept -> ImageType;
        [[nodiscard]] auto samples() const noexcept -> SampleCountFlag;
        [[nodiscard]] auto layers() const noexcept -> core::UInt32;
        [[nodiscard]] auto faces() const noexcept -> core::UInt32;
        [[nodiscard]] auto mipLevels() const noexcept -> core::UInt32;
        [[nodiscard]] auto usages() const noexcept -> ImageUsageFlag;

        [[nodiscard]] operator VkImage() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkImage;

      private:
        struct Tag {};

        Image(const Device &device, const CreateInfo &create_info, Tag);

        core::ExtentU m_extent    = { 0, 0, 0 };
        PixelFormat m_format      = {};
        core::UInt32 m_layers     = 0;
        core::UInt32 m_faces      = 0;
        core::UInt32 m_mip_levels = 0;
        ImageType m_type          = {};
        ImageCreateFlag m_flags   = {};
        SampleCountFlag m_samples = {};
        ImageUsageFlag m_usages   = {};

        VkImage m_image              = VK_NULL_HANDLE;
        VmaAllocation m_image_memory = VK_NULL_HANDLE;

        bool m_own_image = true;
    };
} // namespace stormkit::gpu

#include "Image.inl"
