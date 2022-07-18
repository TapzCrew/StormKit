// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>

#include <stormkit/core/Color.hpp>
#include <stormkit/core/Expected.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>

#include <stormkit/gpu/Fwd.hpp>

#include <stormkit/gpu/resource/ImageSubresourceLayers.hpp>
#include <stormkit/gpu/resource/ImageSubresourceRange.hpp>

namespace stormkit::gpu {
    struct STORMKIT_PUBLIC Viewport {
        core::Vector2f position;
        core::ExtentF extent;
        core::Vector2f depth;

        constexpr operator VkViewport() const noexcept;
    };

    struct STORMKIT_PUBLIC Scissor {
        core::Vector2i offset;
        core::ExtentU extent;

        constexpr operator VkRect2D() const noexcept;
    };

    struct ClearColor {
        core::RGBColorF color = stormkit::core::RGBColorDef::Silver<float>;
    };

    struct ClearDepthStencil {
        float depth          = 1.f;
        core::UInt32 stencil = 0;
    };

    using ClearValue = std::variant<ClearColor, ClearDepthStencil>;

    struct InheritanceInfo {
        const RenderPass *render_pass  = nullptr;
        core::uint32 subpass           = 0;
        const Framebuffer *framebuffer = nullptr;
    };

    struct BufferImageCopy {
        core::UInt32 buffer_offset;
        core::UInt32 buffer_row_length;
        core::UInt32 buffer_image_height;

        ImageSubresourceLayers subresource_layers;

        core::Vector3i offset;
        core::ExtentU extent;
    };

    struct BlitRegion {
        ImageSubresourceLayers source;
        ImageSubresourceLayers destination;

        std::array<core::ExtentI, 2> source_offset;
        std::array<core::ExtentI, 2> destination_offset;
    };

    struct PushConstantRange {
        ShaderStageFlag stages;
        core::UInt32 offset;
        core::USize size;
    };

    struct BufferDescriptor {
        DescriptorType type = DescriptorType::Uniform_Buffer;
        core::UInt32 binding;
        const Buffer *buffer;
        core::UInt32 range;
        core::UInt32 offset;
    };

    struct ImageDescriptor {
        DescriptorType type = DescriptorType::Combined_Image_Sampler;
        core::UInt32 binding;
        ImageLayout layout;
        const ImageView *image_view;
        const Sampler *sampler;
    };

    using Descriptor = std::variant<BufferDescriptor, ImageDescriptor>;
    using ClearValue = std::variant<ClearColor, ClearDepthStencil>;
    using SpirvID    = core::UInt32;

    template<typename T>
    using Expected = core::Expected<T, Result>;
} // namespace stormkit::gpu

HASH_FUNC(stormkit::gpu::Viewport)
HASH_FUNC(stormkit::gpu::Scissor)
HASH_FUNC(stormkit::gpu::BufferDescriptor)
HASH_FUNC(stormkit::gpu::ImageDescriptor)
HASH_FUNC(stormkit::gpu::Descriptor)

#include "Types.inl"
