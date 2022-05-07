// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <fstream>
#include <optional>
#include <variant>
#include <vector>

#include <storm/core/Math.hpp>
#include <storm/core/Types.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/MemoryBarrier.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/TextureSubresourceLayers.hpp>
#include <storm/render/resource/TextureSubresourceRange.hpp>

namespace storm::render {
    struct BeginDebugRegionCommand {
        std::string name;
        core::RGBColorF color = core::RGBColorDef::White<float>;
    };

    struct InsertDebugLabelCommand {
        std::string name;
        core::RGBColorF color = core::RGBColorDef::White<float>;
    };

    struct EndDebugRegionCommand {};

    struct InheritanceInfo {
        RenderPassConstPtr render_pass;
        core::uint32 subpass = 0;
        FramebufferConstPtr framebuffer;
    };

    struct BeginCommand {
        bool one_time_submit                            = false;
        std::optional<InheritanceInfo> inheritence_info = std::nullopt;
    };

    struct EndCommand {};

    struct BeginRenderPassCommand {
        using ClearValues = std::vector<ClearValue>;

        const RenderPass &render_pass;
        const Framebuffer &framebuffer;

        ClearValues clear_values = { ClearColor { .color = core::RGBColorDef::Black<float> } };

        bool secondary_command_buffers = false;
    };

    struct NextSubPassCommand {};

    struct EndRenderPassCommand {};

    struct BindGraphicsPipelineCommand {
        const GraphicsPipeline &pipeline;
    };

    struct BindComputePipelineCommand {
        const ComputePipeline &pipeline;
    };

    struct DispatchCommand {
        core::UInt32 group_count_x;
        core::UInt32 group_count_y;
        core::UInt32 group_count_z;
    };

    struct DrawCommand {
        core::UInt32 vertex_count;
        core::UInt32 instance_count = 1u;
        core::UInt32 first_vertex   = 0u;
        core::UInt32 first_instance = 0u;
    };

    struct DrawIndexedCommand {
        core::UInt32 index_count;
        core::UInt32 instance_count = 2u;
        core::UInt32 first_index    = 0u;
        core::Int32 vertex_offset   = 0;
        core::UInt32 first_instance = 0u;
    };

    struct DrawIndirectCommand {
        const HardwareBuffer &buffer;
        core::Int32 offset;
        core::UInt32 draw_count;
        core::ArraySize stride;
    };

    struct DrawIndexedIndirectCommand {
        const HardwareBuffer &buffer;
        core::Int32 offset;
        core::UInt32 draw_count;
        core::ArraySize stride;
    };

    struct BindVertexBuffersCommand {
        std::vector<HardwareBufferConstRef> buffers;
        std::vector<core::Int32> offsets;
    };

    struct BindIndexBufferCommand {
        const HardwareBuffer &buffer;
        core::Int32 offset = 0u;
        bool large_indices = false;
    };

    struct BindDescriptorSetsCommand {
        const AbstractPipeline *pipeline = nullptr;
        std::vector<DescriptorSetConstRef> descriptor_sets;
        std::vector<core::UInt32> dynamic_offsets;
    };

    struct CopyBufferCommand {
        const HardwareBuffer &source;
        const HardwareBuffer &destination;

        core::ArraySize size;

        core::Int32 src_offset = 0u;
        core::Int32 dst_offset = 0u;
    };

    struct BufferTextureCopy {
        core::UInt32 buffer_offset;
        core::UInt32 buffer_row_length;
        core::UInt32 buffer_image_height;

        TextureSubresourceLayers subresource_layers;

        core::Vector3i offset;
        core::Extentu extent;
    };

    struct CopyBufferToTextureCommand {
        const HardwareBuffer &source;
        const Texture &destination;

        std::vector<BufferTextureCopy> buffer_image_copies;
    };

    struct CopyTextureToBufferCommand {
        const Texture &source;
        const HardwareBuffer &destination;

        std::vector<BufferTextureCopy> buffer_image_copies;
    };

    struct CopyTextureCommand {
        const Texture &source;
        TextureLayout source_layout;
        TextureSubresourceLayers source_subresource_layers;

        const Texture &destination;
        TextureLayout destination_layout;
        TextureSubresourceLayers destination_subresource_layers;

        core::Extentu copy_region;
    };

    struct ResolveTextureCommand {
        const Texture &source;
        TextureLayout source_layout;
        TextureSubresourceLayers source_subresource_layers;

        const Texture &destination;
        TextureLayout destination_layout;
        TextureSubresourceLayers destination_subresource_layers;
    };

    struct BlitRegion {
        TextureSubresourceLayers source;
        TextureSubresourceLayers destination;

        std::array<core::ExtentuOffset, 2> source_offset;
        std::array<core::ExtentuOffset, 2> destination_offset;
    };

    struct BlitTextureCommand {
        const Texture &source;
        TextureLayout source_layout;

        const Texture &destination;
        TextureLayout destination_layout;

        std::vector<BlitRegion> regions;

        Filter filter;
    };

    struct TransitionTextureLayoutCommand {
        const Texture &texture;

        TextureLayout source_layout;
        TextureLayout destination_layout;
        TextureSubresourceRange subresource_range;
    };

    struct ExecuteSubCommandBuffersCommand {
        std::vector<CommandBufferConstRef> command_buffers;
    };

    struct PipelineBarrierCommand {
        PipelineStageFlag src_mask;
        PipelineStageFlag dst_mask;

        DependencyFlag dependency;

        MemoryBarriers memory_barriers;
        BufferMemoryBarriers buffer_memory_barriers;
        ImageMemoryBarriers image_memory_barriers;
    };

    struct SetViewportCommand {
        core::UInt32 first_viewport;
        std::vector<Viewport> viewports;
    };

    struct SetScissorCommand {
        core::UInt32 first_scissor;
        std::vector<Scissor> scissors;
    };

    struct PushConstantsCommand {
        const AbstractPipeline *pipeline = nullptr;
        ShaderStage stage;
        core::UInt32 offset;
        std::vector<std::byte> data;
    };

    using Command = std::variant<BeginDebugRegionCommand,
                                 InsertDebugLabelCommand,
                                 EndDebugRegionCommand,
                                 BeginCommand,
                                 EndCommand,
                                 BeginRenderPassCommand,
                                 NextSubPassCommand,
                                 EndRenderPassCommand,
                                 BindGraphicsPipelineCommand,
                                 BindComputePipelineCommand,
                                 DispatchCommand,
                                 DrawCommand,
                                 DrawIndexedCommand,
                                 DrawIndirectCommand,
                                 DrawIndexedIndirectCommand,
                                 BindVertexBuffersCommand,
                                 BindIndexBufferCommand,
                                 BindDescriptorSetsCommand,
                                 CopyBufferCommand,
                                 CopyBufferToTextureCommand,
                                 CopyTextureToBufferCommand,
                                 CopyTextureCommand,
                                 ResolveTextureCommand,
                                 BlitTextureCommand,
                                 TransitionTextureLayoutCommand,
                                 ExecuteSubCommandBuffersCommand,
                                 SetViewportCommand,
                                 SetScissorCommand,
                                 PipelineBarrierCommand,
                                 PushConstantsCommand>;
} // namespace storm::render
