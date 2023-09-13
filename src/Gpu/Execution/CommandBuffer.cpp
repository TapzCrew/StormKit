// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :CommandBuffer;
import :FrameBuffer;
import :RenderPass;
import :Pipeline;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    namespace details {
        using AccessMapPair =
            std::pair<VkImageLayout, std::pair<VkAccessFlags, VkPipelineStageFlags>>;
    }

    static constexpr auto old_layout_access_map =
        core::makeFrozenMap<VkImageLayout, std::pair<VkAccessFlags, VkPipelineStageFlags>>(
            { { VK_IMAGE_LAYOUT_UNDEFINED,
                { VkAccessFlags {}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT } },
              { VK_IMAGE_LAYOUT_PREINITIALIZED,
                { VkAccessFlags {}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT } },
              { VK_IMAGE_LAYOUT_GENERAL,
                { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } },
              { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } },
              { VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                { VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT } },
              { VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                { VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT } },
              { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                { VK_ACCESS_INPUT_ATTACHMENT_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT } },
              { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                { VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } },
              { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                { VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } },
              { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                { VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } } });

    static constexpr auto new_layout_access_map =
        core::makeFrozenMap<VkImageLayout, std::pair<VkAccessFlags, VkPipelineStageFlags>>(
            { { VK_IMAGE_LAYOUT_UNDEFINED, { VkAccessFlags {}, {} } },
              { VK_IMAGE_LAYOUT_PREINITIALIZED, { VkAccessFlags {}, {} } },
              { VK_IMAGE_LAYOUT_GENERAL,
                { VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
                  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } },
              { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } },
              { VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                { VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT } },
              { VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } },
              { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT } },
              { VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                { VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } },
              { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                { VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } },
              { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                { VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT } } });

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBuffer::CommandBuffer(const Queue& queue,
                                 CommandBufferLevel level,
                                 VkCommandBuffer commandbuffer,
                                 Deleter deleter,
                                 Tag)
        : m_queue { &queue }, m_level { level }, m_commandbuffer { commandbuffer },
          m_deleter { std::move(deleter) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBuffer::~CommandBuffer() {
        if (m_commandbuffer != VK_NULL_HANDLE) [[likely]]
            m_deleter(m_commandbuffer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
        : m_state { std::exchange(other.m_state, State::Initial) },
          m_queue { std::exchange(other.m_queue, nullptr) },
          m_level { std::exchange(other.m_level, CommandBufferLevel::Primary) },
          m_commandbuffer { std::exchange(other.m_commandbuffer, VK_NULL_HANDLE) },
          m_deleter { std::exchange(other.m_deleter, nullptr) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::operator=(CommandBuffer&& other) noexcept -> CommandBuffer& {
        if (&other == this) [[unlikely]]
            return *this;

        m_state         = std::exchange(other.m_state, State::Initial);
        m_queue         = std::exchange(other.m_queue, nullptr);
        m_level         = std::exchange(other.m_level, CommandBufferLevel::Primary);
        m_commandbuffer = std::exchange(other.m_commandbuffer, VK_NULL_HANDLE);
        m_deleter       = std::exchange(other.m_deleter, nullptr);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::reset() noexcept -> void {
        core::expects(m_commandbuffer);
        const auto& vk = m_queue->device().table();

        CHECK_VK_ERROR(vk.vkResetCommandBuffer(m_commandbuffer, {}));

        m_state = State::Initial;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::submit(std::span<const Semaphore *> wait_semaphores,
                               std::span<const Semaphore *> signal_semaphores,
                               Fence *fence) const noexcept -> void {
        core::expects(m_state == State::Executable);

        auto cmb = core::makeConstObserverStaticArray(this);
        m_queue->submit(cmb, wait_semaphores, signal_semaphores, fence);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::beginDebugRegion(std::string_view name, const core::RGBColorF& color)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        if (!vkCmdBeginDebugUtilsLabelEXT or !vkCmdEndDebugUtilsLabelEXT or
            !vkCmdInsertDebugUtilsLabelEXT)
            return;

        const auto payload =
            VkDebugUtilsLabelEXT { .sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                                   .pLabelName = std::data(name),
                                   .color = { color.red, color.green, color.blue, color.alpha } };

        vkCmdBeginDebugUtilsLabelEXT(m_commandbuffer, &payload);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::insertDebugLabel(std::string_view name, const core::RGBColorF& color)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        if (!vkCmdBeginDebugUtilsLabelEXT or !vkCmdEndDebugUtilsLabelEXT or
            !vkCmdInsertDebugUtilsLabelEXT)
            return;

        const auto payload =
            VkDebugUtilsLabelEXT { .sType      = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
                                   .pLabelName = std::data(name),
                                   .color = { color.red, color.green, color.blue, color.alpha } };

        vkCmdInsertDebugUtilsLabelEXT(m_commandbuffer, &payload);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::endDebugRegion() -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        if (!vkCmdBeginDebugUtilsLabelEXT or !vkCmdEndDebugUtilsLabelEXT or
            !vkCmdInsertDebugUtilsLabelEXT)
            return;

        vkCmdEndDebugUtilsLabelEXT(m_commandbuffer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::begin(bool one_time_submit, std::optional<InheritanceInfo> inheritance_info)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Initial);

        const auto& vk = m_queue->device().table();

        const auto _inheritance_info = [&] {
            auto i = VkCommandBufferInheritanceInfo {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            };

            if (inheritance_info.has_value())
                i = VkCommandBufferInheritanceInfo {
                    .sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
                    .renderPass  = *inheritance_info->render_pass,
                    .subpass     = inheritance_info->subpass,
                    .framebuffer = *inheritance_info->framebuffer
                };

            return i;
        }();

        const auto flags = [&] {
            auto f = VkCommandBufferUsageFlags { VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };

            if (!one_time_submit) f = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            if (m_level == CommandBufferLevel::Secondary)
                f |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

            return f;
        }();

        const auto begin_info =
            VkCommandBufferBeginInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                       .flags = flags,
                                       .pInheritanceInfo = &_inheritance_info };

        CHECK_VK_ERROR(vk.vkBeginCommandBuffer(m_commandbuffer, &begin_info));
        m_state = State::Recording;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::end() -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();

        CHECK_VK_ERROR(vk.vkEndCommandBuffer(m_commandbuffer));
        m_state = State::Executable;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::beginRenderPass(const RenderPass& render_pass,
                                        const FrameBuffer& framebuffer,
                                        std::span<const ClearValue> clear_values,
                                        bool secondary_commandbuffers) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();

        const auto _clear_values = core::transform(clear_values, [](const auto& clear_value) {
            auto out = VkClearValue {};

            if (core::is<ClearColor>(clear_value)) {
                const auto& clear_color = core::as<ClearColor>(clear_value);
                out.color               = VkClearColorValue { .float32 = { clear_color.color.red,
                                                                           clear_color.color.green,
                                                                           clear_color.color.blue,
                                                                           clear_color.color.alpha } };
            } else {
                const auto& clear_depth_stencil = core::as<ClearDepthStencil>(clear_value);
                out.depthStencil =
                    VkClearDepthStencilValue { .stencil = clear_depth_stencil.stencil };
            }

            return out;
        });

        const auto begin_info = VkRenderPassBeginInfo {
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass  = render_pass,
            .framebuffer = framebuffer,
            .renderArea =
                VkRect2D { .offset = { 0, 0 },
                           .extent = { framebuffer.extent().width, framebuffer.extent().height } },
            .clearValueCount = core::as<core::UInt32>(std::size(_clear_values)),
            .pClearValues    = std::data(_clear_values),
        };

        vk.vkCmdBeginRenderPass(m_commandbuffer,
                                &begin_info,
                                (secondary_commandbuffers)
                                    ? VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
                                    : VK_SUBPASS_CONTENTS_INLINE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::nextSubPass() -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdNextSubpass(m_commandbuffer, VK_SUBPASS_CONTENTS_INLINE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::endRenderPass() -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdEndRenderPass(m_commandbuffer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::bindPipeline(const Pipeline& pipeline) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        pipeline.bind(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setViewport(core::UInt32 first_viewport,
                                    std::span<const Viewport> viewports) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        const auto _viewports =
            core::transform(viewports, [](const auto& v) { return core::as<VkViewport>(v); });

        vk.vkCmdSetViewport(m_commandbuffer,
                            first_viewport,
                            core::as<core::UInt32>(std::size(_viewports)),
                            std::data(_viewports));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setScissor(core::UInt32 first_scissor, std::span<const Scissor> scissors)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        const auto _scissors =
            core::transform(scissors, [](const auto& s) { return core::as<VkRect2D>(s); });

        vk.vkCmdSetScissor(m_commandbuffer,
                           first_scissor,
                           core::as<core::UInt32>(std::size(_scissors)),
                           std::data(_scissors));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setLineWidth(float width) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetLineWidth(m_commandbuffer, width);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setDepthBias(float constant_factor, float clamp, float slope_factor)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetDepthBias(m_commandbuffer, constant_factor, clamp, slope_factor);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setBlendConstants(std::span<const float> constants) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetBlendConstants(m_commandbuffer, std::data(constants));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setDepthBounds(float min, float max) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetDepthBounds(m_commandbuffer, min, max);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setStencilCompareMask(StencilFaceFlag face, core::UInt32 mask) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetStencilCompareMask(m_commandbuffer, core::as<VkStencilFaceFlags>(face), mask);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setStencilWriteMask(StencilFaceFlag face, core::UInt32 mask) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetStencilWriteMask(m_commandbuffer, core::as<VkStencilFaceFlags>(face), mask);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::setStencilReference(StencilFaceFlag face, core::UInt32 reference) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdSetStencilReference(m_commandbuffer, core::as<VkStencilFaceFlags>(face), reference);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::CommandBuffer::dispatch(core::UInt32 group_count_x,
                                                core::UInt32 group_count_y,
                                                core::UInt32 group_count_z) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdDispatch(m_commandbuffer, group_count_x, group_count_y, group_count_z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::draw(core::UInt32 vertex_count,
                             core::UInt32 instance_count,
                             core::UInt32 first_vertex,
                             core::UInt32 first_instance) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(vertex_count > 0u);

        const auto& vk = m_queue->device().table();
        vk.vkCmdDraw(m_commandbuffer, vertex_count, instance_count, first_vertex, first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::drawIndexed(core::UInt32 index_count,
                                    core::UInt32 instance_count,
                                    core::UInt32 first_index,
                                    core::Int32 vertex_offset,
                                    core::UInt32 first_instance) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(index_count > 0u);

        const auto& vk = m_queue->device().table();
        vk.vkCmdDrawIndexed(m_commandbuffer,
                            index_count,
                            instance_count,
                            first_index,
                            vertex_offset,
                            first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::drawIndirect(const Buffer& buffer,
                                     core::RangeExtent offset,
                                     core::UInt32 draw_count,
                                     core::UInt32 stride) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(draw_count > 0u);

        const auto& vk = m_queue->device().table();
        vk.vkCmdDrawIndirect(m_commandbuffer, buffer, offset, draw_count, stride);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::drawIndexedIndirect(const Buffer& buffer,
                                            core::RangeExtent offset,
                                            core::UInt32 draw_count,
                                            core::UInt32 stride) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(draw_count > 0u);

        const auto& vk = m_queue->device().table();
        vk.vkCmdDrawIndexedIndirect(m_commandbuffer, buffer, offset, draw_count, stride);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::bindVertexBuffers(std::span<const Buffer *> buffers,
                                          std::span<const core::UInt64> offsets) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(!std::empty(buffers));
        core::expects(!std::empty(offsets));
        core::expects(std::size(buffers) == std::size(offsets));

        const auto& vk = m_queue->device().table();

        const auto _buffers = core::transform(buffers, getHandle<const Buffer *>);

        const auto _offsets =
            core::transform(offsets, [](const auto& o) { return core::as<VkDeviceSize>(o); });

        vk.vkCmdBindVertexBuffers(m_commandbuffer,
                                  0,
                                  core::as<core::UInt32>(std::size(_buffers)),
                                  std::data(_buffers),
                                  std::data(_offsets));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::bindIndexBuffer(const Buffer& buffer,
                                        core::UInt64 offset,
                                        bool large_indices) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto& vk = m_queue->device().table();
        vk.vkCmdBindIndexBuffer(m_commandbuffer,
                                buffer,
                                core::as<VkDeviceSize>(offset),
                                (large_indices) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::bindDescriptorSets(const RasterPipeline& pipeline,
                                           std::span<const DescriptorSet *> descriptor_sets,
                                           std::span<const core::UInt32> dynamic_offsets) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(!std::empty(descriptor_sets));

        const auto _descriptor_sets =
            core::transform(descriptor_sets, getHandle<const DescriptorSet *>);

        const auto& vk = m_queue->device().table();
        vk.vkCmdBindDescriptorSets(m_commandbuffer,
                                   VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   pipeline.vkLayout(),
                                   0,
                                   core::as<core::UInt32>(std::size(_descriptor_sets)),
                                   std::data(_descriptor_sets),
                                   core::as<core::UInt32>(std::size(dynamic_offsets)),
                                   std::data(dynamic_offsets));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::bindDescriptorSets(const ComputePipeline& pipeline,
                                           std::span<const DescriptorSet *> descriptor_sets,
                                           std::span<const core::UInt32> dynamic_offsets) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto _descriptor_sets =
            core::transform(descriptor_sets, getHandle<const DescriptorSet *>);

        const auto& vk = m_queue->device().table();
        vk.vkCmdBindDescriptorSets(m_commandbuffer,
                                   VK_PIPELINE_BIND_POINT_COMPUTE,
                                   pipeline.vkLayout(),
                                   0,
                                   core::as<core::UInt32>(std::size(_descriptor_sets)),
                                   std::data(_descriptor_sets),
                                   core::as<core::UInt32>(std::size(dynamic_offsets)),
                                   std::data(dynamic_offsets));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::copyBuffer(const Buffer& source,
                                   const Buffer& destination,
                                   core::RangeExtent size,
                                   core::UInt64 src_offset,
                                   core::UInt64 dst_offset) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto copy_buffers = VkBufferCopy { .srcOffset = core::as<VkDeviceSize>(src_offset),
                                                 .dstOffset = core::as<VkDeviceSize>(dst_offset),
                                                 .size      = size };

        const auto& vk = m_queue->device().table();
        vk.vkCmdCopyBuffer(m_commandbuffer, source, destination, 1, &copy_buffers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::copyBufferToImage(const Buffer& source,
                                          const Image& destination,
                                          std::span<const BufferImageCopy> buffer_image_copies)
        -> void {
        const auto default_copy =
            std::array { BufferImageCopy { 0, 0, 0, {}, { 0, 0, 0 }, destination.extent() } };

        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto copy_regions =
            core::transform((std::empty(buffer_image_copies)) ? default_copy : buffer_image_copies,
                            [](const auto& buffer_image_copy) {
                                const auto image_subresource = VkImageSubresourceLayers {
                                    .aspectMask = core::as<VkImageAspectFlags>(
                                        buffer_image_copy.subresource_layers.aspect_mask),
                                    .mipLevel = buffer_image_copy.subresource_layers.mip_level,
                                    .baseArrayLayer =
                                        buffer_image_copy.subresource_layers.base_array_layer,
                                    .layerCount = buffer_image_copy.subresource_layers.layer_count
                                };

                                return VkBufferImageCopy {
                                    .bufferOffset      = buffer_image_copy.buffer_offset,
                                    .bufferRowLength   = buffer_image_copy.buffer_row_length,
                                    .bufferImageHeight = buffer_image_copy.buffer_image_height,
                                    .imageSubresource  = image_subresource,
                                    .imageOffset       = { buffer_image_copy.offset.x,
                                                           buffer_image_copy.offset.y,
                                                           buffer_image_copy.offset.z },
                                    .imageExtent       = { buffer_image_copy.extent.width,
                                                           buffer_image_copy.extent.height,
                                                           buffer_image_copy.extent.depth }
                                };
                            });

        const auto& vk = m_queue->device().table();
        vk.vkCmdCopyBufferToImage(m_commandbuffer,
                                  source,
                                  destination,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  core::as<core::UInt32>(std::size(copy_regions)),
                                  std::data(copy_regions));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::copyImageToBuffer(const Image& source,
                                          const Buffer& destination,
                                          std::span<const BufferImageCopy> buffer_image_copies)
        -> void {
        const auto default_copy =
            std::array { BufferImageCopy { 0, 0, 0, {}, { 0, 0, 0 }, source.extent() } };

        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto copy_regions =
            core::transform((std::empty(buffer_image_copies)) ? default_copy : buffer_image_copies,
                            [](const auto& buffer_image_copy) {
                                const auto image_subresource = VkImageSubresourceLayers {
                                    .aspectMask = core::as<VkImageAspectFlags>(
                                        buffer_image_copy.subresource_layers.aspect_mask),
                                    .mipLevel = buffer_image_copy.subresource_layers.mip_level,
                                    .baseArrayLayer =
                                        buffer_image_copy.subresource_layers.base_array_layer,
                                    .layerCount = buffer_image_copy.subresource_layers.layer_count
                                };

                                return VkBufferImageCopy {
                                    .bufferOffset      = buffer_image_copy.buffer_offset,
                                    .bufferRowLength   = buffer_image_copy.buffer_row_length,
                                    .bufferImageHeight = buffer_image_copy.buffer_image_height,
                                    .imageSubresource  = image_subresource,
                                    .imageOffset       = { buffer_image_copy.offset.x,
                                                           buffer_image_copy.offset.y,
                                                           buffer_image_copy.offset.z },
                                    .imageExtent       = { buffer_image_copy.extent.width,
                                                           buffer_image_copy.extent.height,
                                                           buffer_image_copy.extent.depth }
                                };
                            });

        const auto& vk = m_queue->device().table();
        vk.vkCmdCopyImageToBuffer(m_commandbuffer,
                                  source,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  destination,
                                  core::as<core::UInt32>(std::size(copy_regions)),
                                  std::data(copy_regions));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::copyImage(const Image& source,
                                  const Image& destination,
                                  ImageLayout source_layout,
                                  ImageLayout destination_layout,
                                  const ImageSubresourceLayers& source_subresource_layers,
                                  const ImageSubresourceLayers& destination_subresource_layers,
                                  const core::math::ExtentU& extent) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto _extent = VkExtent3D { extent.width, extent.height, 1 };

        const auto src_subresource = VkImageSubresourceLayers {
            .aspectMask     = core::as<VkImageAspectFlags>(source_subresource_layers.aspect_mask),
            .mipLevel       = source_subresource_layers.mip_level,
            .baseArrayLayer = source_subresource_layers.base_array_layer,
            .layerCount     = source_subresource_layers.layer_count
        };

        const auto dst_subresource = VkImageSubresourceLayers {
            .aspectMask = core::as<VkImageAspectFlags>(destination_subresource_layers.aspect_mask),
            .mipLevel   = destination_subresource_layers.mip_level,
            .baseArrayLayer = destination_subresource_layers.base_array_layer,
            .layerCount     = destination_subresource_layers.layer_count
        };

        const auto regions = std::array { VkImageCopy { .srcSubresource = src_subresource,
                                                        .srcOffset      = VkOffset3D { 0, 0, 0 },
                                                        .dstSubresource = dst_subresource,
                                                        .dstOffset      = VkOffset3D { 0, 0, 0 },
                                                        .extent         = _extent } };

        const auto& vk = m_queue->device().table();
        vk.vkCmdCopyImage(m_commandbuffer,
                          source,
                          core::as<VkImageLayout>(source_layout),
                          destination,
                          core::as<VkImageLayout>(destination_layout),
                          core::as<core::UInt32>(std::size(regions)),
                          std::data(regions));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::resolveImage(const Image& source,
                                     const Image& destination,
                                     ImageLayout source_layout,
                                     ImageLayout destination_layout,
                                     const ImageSubresourceLayers& source_subresource_layers,
                                     const ImageSubresourceLayers& destination_subresource_layers)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto extent =
            VkExtent3D { destination.extent().width, destination.extent().height, 1 };

        const auto src_subresource = VkImageSubresourceLayers {
            .aspectMask     = core::as<VkImageAspectFlags>(source_subresource_layers.aspect_mask),
            .mipLevel       = source_subresource_layers.mip_level,
            .baseArrayLayer = source_subresource_layers.base_array_layer,
            .layerCount     = source_subresource_layers.layer_count,
        };

        const auto dst_subresource = VkImageSubresourceLayers {
            .aspectMask = core::as<VkImageAspectFlags>(destination_subresource_layers.aspect_mask),
            .mipLevel   = destination_subresource_layers.mip_level,
            .baseArrayLayer = destination_subresource_layers.base_array_layer,
            .layerCount     = destination_subresource_layers.layer_count
        };

        const auto regions = std::array { VkImageResolve { .srcSubresource = src_subresource,
                                                           .srcOffset      = VkOffset3D { 0, 0, 0 },
                                                           .dstSubresource = dst_subresource,
                                                           .dstOffset      = VkOffset3D { 0, 0, 0 },
                                                           .extent         = extent } };

        const auto& vk = m_queue->device().table();
        vk.vkCmdResolveImage(m_commandbuffer,
                             source,
                             core::as<VkImageLayout>(source_layout),
                             destination,
                             core::as<VkImageLayout>(destination_layout),
                             core::as<core::UInt32>(std::size(regions)),
                             std::data(regions));
    }

    auto CommandBuffer::blitImage(const Image& source,
                                  const Image& destination,
                                  ImageLayout source_layout,
                                  ImageLayout destination_layout,
                                  std::span<const BlitRegion> regions,
                                  Filter filter) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto _regions = core::transform(regions, [](const auto& region) {
            const auto src_subresource = VkImageSubresourceLayers {
                .aspectMask     = core::as<VkImageAspectFlags>(region.source.aspect_mask),
                .mipLevel       = region.source.mip_level,
                .baseArrayLayer = region.source.base_array_layer,
                .layerCount     = region.source.layer_count,
            };

            const auto dst_subresource = VkImageSubresourceLayers {
                .aspectMask     = core::as<VkImageAspectFlags>(region.destination.aspect_mask),
                .mipLevel       = region.destination.mip_level,
                .baseArrayLayer = region.destination.base_array_layer,
                .layerCount     = region.destination.layer_count,
            };

            return VkImageBlit { .srcSubresource = src_subresource,
                                 .srcOffsets     = {
                                    VkOffset3D { .x = region.source_offset[0].width,
                                                 .y = region.source_offset[0].height,
                                                 .z = region.source_offset[0].depth },
                                          VkOffset3D { .x = region.source_offset[1].width,
                                                       .y = region.source_offset[1].height,
                                                       .z = region.source_offset[1].depth },
                                     },
                                     .dstSubresource = dst_subresource,
                                     .dstOffsets     = {
                                          VkOffset3D { .x = region.destination_offset[0].width,
                                                       .y = region.destination_offset[0].height,
                                                       .z = region.destination_offset[0].depth },
                                          VkOffset3D { .x = region.destination_offset[1].width,
                                                       .y = region.destination_offset[1].height,
                                                       .z = region.destination_offset[1].depth }
                                     }
                                   };
        });

        const auto& vk = m_queue->device().table();
        vk.vkCmdBlitImage(m_commandbuffer,
                          source,
                          core::as<VkImageLayout>(source_layout),
                          destination,
                          core::as<VkImageLayout>(destination_layout),
                          core::as<core::UInt32>(std::size(_regions)),
                          std::data(_regions),
                          core::as<VkFilter>(filter));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::transitionImageLayout(const Image& image,
                                              ImageLayout source_layout,
                                              ImageLayout destination_layout,
                                              const ImageSubresourceRange& subresource_range)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto src_layout = core::as<VkImageLayout>(source_layout);
        const auto dst_layout = core::as<VkImageLayout>(destination_layout);

        const auto& src_access = old_layout_access_map.find(src_layout);
        const auto& dst_access = new_layout_access_map.find(dst_layout);

        const auto src_stage = src_access->second.second;
        const auto dst_stage = dst_access->second.second;

        const auto _subresource_range = VkImageSubresourceRange {
            .aspectMask     = core::as<VkImageAspectFlags>(subresource_range.aspect_mask),
            .baseMipLevel   = subresource_range.base_mip_level,
            .levelCount     = subresource_range.level_count,
            .baseArrayLayer = subresource_range.base_array_layer,
            .layerCount     = subresource_range.layer_count,
        };

        const auto barriers =
            std::array { VkImageMemoryBarrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                .srcAccessMask       = src_access->second.first,
                                                .dstAccessMask       = dst_access->second.first,
                                                .oldLayout           = src_layout,
                                                .newLayout           = dst_layout,
                                                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                .image               = image,
                                                .subresourceRange    = _subresource_range } };

        const auto& vk = m_queue->device().table();
        vk.vkCmdPipelineBarrier(m_commandbuffer,
                                src_stage,
                                dst_stage,
                                {},
                                0,
                                nullptr,
                                0,
                                nullptr,
                                core::as<core::UInt32>(std::size(barriers)),
                                std::data(barriers));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::executeSubCommandBuffers(std::span<const CommandBuffer *> commandbuffers)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto _commandbuffers =
            core::transform(commandbuffers, [](const auto& cmb) -> VkCommandBuffer {
                core::expects(cmb->level() == gpu::CommandBufferLevel::Secondary);

                return *cmb;
            });

        const auto& vk = m_queue->device().table();
        vk.vkCmdExecuteCommands(m_commandbuffer,
                                core::as<core::UInt32>(std::size(_commandbuffers)),
                                std::data(_commandbuffers));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::pipelineBarrier(PipelineStageFlag src_mask,
                                        PipelineStageFlag dst_mask,
                                        [[maybe_unused]] DependencyFlag dependency,
                                        std::span<const MemoryBarrier> memory_barriers,
                                        std::span<const BufferMemoryBarrier> buffer_memory_barriers,
                                        std::span<const ImageMemoryBarrier> image_memory_barriers)
        -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);

        const auto _memory_barriers = core::transform(memory_barriers, [](const auto& barrier) {
            return VkMemoryBarrier { .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
                                     .srcAccessMask = core::as<VkAccessFlags>(barrier.src),
                                     .dstAccessMask = core::as<VkAccessFlags>(barrier.dst) };
        });

        const auto _buffer_memory_barriers =
            core::transform(buffer_memory_barriers, [](const auto& barrier) {
                return VkBufferMemoryBarrier {
                    .sType               = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .srcAccessMask       = core::as<VkAccessFlags>(barrier.src),
                    .dstAccessMask       = core::as<VkAccessFlags>(barrier.dst),
                    .srcQueueFamilyIndex = barrier.src_queue_family_index,
                    .dstQueueFamilyIndex = barrier.dst_queue_family_index,
                    .buffer              = barrier.buffer,
                    .offset              = barrier.offset,
                    .size                = barrier.size
                };
            });

        const auto _image_memory_barriers =
            core::transform(image_memory_barriers, [](const auto& barrier) {
                const auto vk_subresource_range = VkImageSubresourceRange {
                    .aspectMask     = core::as<VkAccessFlags>(barrier.range.aspect_mask),
                    .baseMipLevel   = barrier.range.base_mip_level,
                    .levelCount     = barrier.range.level_count,
                    .baseArrayLayer = barrier.range.base_array_layer,
                    .layerCount     = barrier.range.layer_count
                };

                return VkImageMemoryBarrier { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                              .srcAccessMask = core::as<VkAccessFlags>(barrier.src),
                                              .dstAccessMask = core::as<VkAccessFlags>(barrier.dst),
                                              .oldLayout =
                                                  core::as<VkImageLayout>(barrier.old_layout),
                                              .newLayout =
                                                  core::as<VkImageLayout>(barrier.new_layout),
                                              .srcQueueFamilyIndex = barrier.src_queue_family_index,
                                              .dstQueueFamilyIndex = barrier.dst_queue_family_index,
                                              .image               = barrier.image,
                                              .subresourceRange    = vk_subresource_range };
            });

        const auto& vk = m_queue->device().table();
        vk.vkCmdPipelineBarrier(m_commandbuffer,
                                core::as<VkPipelineStageFlags>(src_mask),
                                core::as<VkPipelineStageFlags>(src_mask),
                                core::as<VkDependencyFlags>(dst_mask),
                                core::as<core::UInt32>(std::size(_memory_barriers)),
                                std::data(_memory_barriers),
                                core::as<core::UInt32>(std::size(_buffer_memory_barriers)),
                                std::data(_buffer_memory_barriers),
                                core::as<core::UInt32>(std::size(_image_memory_barriers)),
                                std::data(_image_memory_barriers));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBuffer::pushConstants(const Pipeline& pipeline,
                                      ShaderStageFlag stage,
                                      std::span<const core::Byte> data,
                                      core::UInt32 offset) -> void {
        core::expects(m_commandbuffer);
        core::expects(m_state == State::Recording);
        core::expects(pipeline != nullptr);
        core::expects(!std::empty(data));

        const auto& vk = m_queue->device().table();
        vk.vkCmdPushConstants(m_commandbuffer,
                              pipeline.vkLayout(),
                              core::as<VkShaderStageFlags>(stage),
                              offset,
                              core::as<core::UInt32>(std::size(data)),
                              std::data(data));
    }
} // namespace stormkit::gpu
