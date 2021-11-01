// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <queue>
#include <unordered_map>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Command.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/pipeline/ComputePipeline.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC CommandBuffer: public core::NonCopyable {
      public:
        enum class State { Initial, Recording, Executable };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Command_Buffer;

        CommandBuffer(const render::Queue &queue,
                      render::CommandBufferLevel level,
                      RAIIVkCommandBuffer &&command_buffer);
        ~CommandBuffer();

        CommandBuffer(CommandBuffer &&);
        CommandBuffer &operator=(CommandBuffer &&);

        void reset() noexcept;
        void build();
        void submit(storm::core::span<const SemaphoreConstPtr> wait_semaphores   = {},
                    storm::core::span<const SemaphoreConstPtr> signal_semaphores = {},
                    FencePtr fence = nullptr) const noexcept;

        State state() const noexcept;
        CommandBufferLevel level() const noexcept;

        template<typename CommandT>
        void add(CommandT &&command);
        template<typename CommandT, typename... Args>
        void add(Args &&...args);

        void beginDebugRegion(std::string_view name,
                              core::RGBColorF color = core::RGBColorDef::White<float>);
        void insertDebugLabel(std::string_view name,
                              core::RGBColorF color = core::RGBColorDef::White<float>);
        void endDebugRegion();

        void begin(bool one_time_submit                            = false,
                   std::optional<InheritanceInfo> inheritence_info = std::nullopt);
        void end();

        void beginRenderPass(const RenderPass &render_pass,
                             const Framebuffer &framebuffer,
                             BeginRenderPassCommand::ClearValues clear_values = { ClearColor {
                                 .color = core::RGBColorDef::Silver<float> } },
                             bool secondary_command_buffers                   = false);
        void nextSubPass();
        void endRenderPass();

        void bindGraphicsPipeline(const GraphicsPipeline &pipeline);
        void bindComputePipeline(const ComputePipeline &pipeline);

        void dispatch(core::UInt32 group_count_x,
                      core::UInt32 group_count_y,
                      core::UInt32 group_count_z);

        void draw(core::UInt32 vertex_count,
                  core::UInt32 instance_count = 1u,
                  core::UInt32 first_vertex   = 0,
                  core::UInt32 first_instance = 0);
        void drawIndexed(core::UInt32 index_count,
                         core::UInt32 instance_count = 1u,
                         core::UInt32 first_index    = 0u,
                         core::Int32 vertex_offset   = 0,
                         core::UInt32 first_instance = 0u);
        void drawIndirect(const HardwareBuffer &buffer,
                          core::Int32 offset,
                          core::UInt32 draw_count,
                          core::ArraySize stride);
        void drawIndexedIndirect(const HardwareBuffer &buffer,
                                 core::Int32 offset,
                                 core::UInt32 draw_count,
                                 core::ArraySize stride);

        void bindVertexBuffers(std::vector<HardwareBufferConstRef> buffers,
                               std::vector<core::Int32> Int32s);
        void bindIndexBuffer(const HardwareBuffer &buffer,
                             core::Int32 offset = 0,
                             bool large_indices = false);
        void bindDescriptorSets(const GraphicsPipeline &pipeline,
                                std::vector<DescriptorSetConstRef> descriptor_sets,
                                std::vector<core::UInt32> dynamic_offsets = {});
        void bindDescriptorSets(const ComputePipeline &pipeline,
                                std::vector<DescriptorSetConstRef> descriptor_sets,
                                std::vector<core::UInt32> dynamic_offsets = {});

        void copyBuffer(const HardwareBuffer &source,
                        const HardwareBuffer &destination,
                        core::ArraySize size,
                        core::Int32 src_offset = 0u,
                        core::Int32 dst_offset = 0u);
        void copyBufferToTexture(const HardwareBuffer &source,
                                 const Texture &destination,
                                 std::vector<BufferTextureCopy> buffer_image_copies = {});
        void copyTextureToBuffer(const Texture &source,
                                 const HardwareBuffer &destination,
                                 std::vector<BufferTextureCopy> buffer_image_copies = {});
        void copyTexture(const Texture &source,
                         const Texture &destination,
                         TextureLayout source_layout,
                         TextureLayout destination_layout,
                         TextureSubresourceLayers source_subresource_layers,
                         TextureSubresourceLayers destination_subresource_layers,
                         core::Extentu extent);

        void resolveTexture(const Texture &source,
                            const Texture &destination,
                            TextureLayout source_layout,
                            TextureLayout destination_layout,
                            TextureSubresourceLayers source_subresource_layers      = {},
                            TextureSubresourceLayers destination_subresource_layers = {});

        void blitTexture(const Texture &source,
                         const Texture &destination,
                         TextureLayout source_layout,
                         TextureLayout destination_layout,
                         std::vector<BlitRegion> regions,
                         Filter filter);

        void transitionTextureLayout(const Texture &texture,
                                     TextureLayout source_layout,
                                     TextureLayout destination_layout,
                                     TextureSubresourceRange subresource_range = {});

        void executeSubCommandBuffers(std::vector<CommandBufferConstRef> command_buffers);

        void setViewport(core::UInt32 first_viewport, std::vector<Viewport> viewports);

        void setScissor(core::UInt32 first_scissor, std::vector<Scissor> scissors);

        void pipelineBarrier(PipelineStageFlag src_mask,
                             PipelineStageFlag dst_mask,
                             DependencyFlag dependency,
                             MemoryBarriers memory_barriers,
                             BufferMemoryBarriers buffer_memory_barriers,
                             ImageMemoryBarriers image_memory_barriers);

        void pushConstants(const GraphicsPipeline &pipeline,
                           ShaderStage stage,
                           std::vector<std::byte> data,
                           core::UInt32 offset = 0u);
        void pushConstants(const ComputePipeline &pipeline,
                           ShaderStage stage,
                           std::vector<std::byte> data,
                           core::UInt32 offset = 0u);

        vk::CommandBuffer vkCommandBuffer() const noexcept;
        operator vk::CommandBuffer() const noexcept;
        vk::CommandBuffer vkHandle() const noexcept;
        core::UInt64 vkDebugHandle() const noexcept;

      private:
        State m_state = State::Initial;
        QueueConstPtr m_queue;
        CommandBufferLevel m_level;

        std::queue<Command> m_commands;

        RAIIVkCommandBuffer m_vk_command_buffer;

        core::HashMap<TextureConstPtr, TextureLayout> m_to_update_texture_layout;

        core::HashSet<std::string> m_debug_labels;
    };
} // namespace storm::render

#include "CommandBuffer.inl"
