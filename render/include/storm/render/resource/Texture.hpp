// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/image/Image.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/TextureView.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Texture: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Image;

        Texture(const Device &device,
                core::Extentu extent,
                render::PixelFormat format = render::PixelFormat::RGBA8_UNorm,
                core::UInt32 layers        = 1u,
                core::UInt32 mip_levels    = 1u,
                TextureType type           = TextureType::T2D,
                TextureCreateFlag flags    = TextureCreateFlag::None,
                SampleCountFlag samples    = SampleCountFlag::C1_BIT,
                TextureUsage usage         = TextureUsage::Sampled | TextureUsage::Transfert_Dst |
                                     TextureUsage::Transfert_Src,
                TextureTiling tiling = TextureTiling::Optimal,
                MemoryProperty property = MemoryProperty::Device_Local);
        Texture(const Device &device,
                core::Extentu extent,
                render::PixelFormat format,
                vk::Image image);
        ~Texture();

        Texture(Texture &&);
        Texture &operator=(Texture &&);

        void loadFromImage(const image::Image &image, bool generate_mips = false);
        void loadFromImage(const image::Image &image,
                           render::CommandBuffer &command_buffer,
                           render::HardwareBuffer &buffer,
                           core::UInt32 offset = 0u,
                           bool generate_mips  = false);

        void loadFromMemory(core::ByteConstSpan data,
                            core::UInt32 layers,
                            core::UInt32 faces,
                            core::UInt32 mip_levels,
                            bool generate_mips = false);
        void loadFromMemory(core::ByteConstSpan data,
                            core::UInt32 layers,
                            core::UInt32 faces,
                            core::UInt32 mip_levels,
                            render::CommandBuffer &command_buffer,
                            render::HardwareBuffer &buffer,
                            core::UInt32 offset = 0u,
                            bool generate_mips  = false);

        [[nodiscard]] TextureView
            createView(TextureViewType type                      = TextureViewType::T2D,
                       TextureSubresourceRange subresource_range = {}) const noexcept;
        [[nodiscard]] TextureViewOwnedPtr
            createViewPtr(TextureViewType type                      = TextureViewType::T2D,
                          TextureSubresourceRange subresource_range = {}) const;

        void generateMipmap(render::CommandBuffer &cmb, core::UInt32 mip_level);

        [[nodiscard]] inline core::Extentu extent() const noexcept;
        [[nodiscard]] inline PixelFormat format() const noexcept;
        [[nodiscard]] inline TextureType type() const noexcept;
        [[nodiscard]] inline SampleCountFlag samples() const noexcept;
        [[nodiscard]] inline core::UInt32 layers() const noexcept;
        [[nodiscard]] inline core::UInt32 faces() const noexcept;
        [[nodiscard]] inline core::UInt32 mipLevels() const noexcept;
        [[nodiscard]] inline TextureUsage usage() const noexcept;

        [[nodiscard]] inline const Device &device() const noexcept;

        [[nodiscard]] inline vk::Image vkImage() const noexcept;
        [[nodiscard]] inline operator vk::Image() const noexcept;
        [[nodiscard]] inline vk::Image vkHandle() const noexcept;
        [[nodiscard]] inline core::UInt64 vkDebugHandle() const noexcept;
        [[nodiscard]] VmaAllocation vkAllocation() const noexcept { return m_vma_texture_memory.handle(); }
      private:
        DeviceConstPtr m_device;

        core::Extentu m_extent;
        PixelFormat m_format;
        core::UInt32 m_layers;
        core::UInt32 m_faces = 1u;
        core::UInt32 m_mip_levels;
        TextureType m_type;
        TextureCreateFlag m_flags;
        SampleCountFlag m_samples;
        TextureUsage m_usage;

        RAIIVmaAllocation m_vma_texture_memory;
        RAIIVkImage m_vk_texture;
        vk::Image m_non_owning_texture = VK_NULL_HANDLE;
    };
} // namespace storm::render

#include "Texture.inl"
