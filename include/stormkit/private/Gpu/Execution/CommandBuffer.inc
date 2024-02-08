// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::gpu {
    class STORMKIT_API Queue: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Queue;

        Queue(const Device& device, const Device::QueueEntry& entry, Tag);
        ~Queue();

        Queue(const Queue&)                   = delete;
        auto operator=(const Queue&) noexcept = delete;

        Queue(Queue&&) noexcept;
        auto operator=(Queue&&) noexcept -> Queue&;

        [[nodiscard]] static auto create(const Device&             device,
                                         const Device::QueueEntry& entry) noexcept
            -> Expected<Queue>;
        [[nodiscard]] static auto allocate(const Device&             device,
                                           const Device::QueueEntry& entry) noexcept
            -> Expected<std::unique_ptr<Queue>>;

        auto waitIdle() const noexcept -> void;

        auto submit(std::span<core::NakedRef<const CommandBuffer>> commandbuffers,
                    std::span<core::NakedRef<const Semaphore>>     wait_semaphores   = {},
                    std::span<core::NakedRef<const Semaphore>>     signal_semaphores = {},
                    core::OptionalRef<const Fence> fence = std::nullopt) const noexcept -> void;

        [[nodiscard]] auto entry() const noexcept -> const Device::QueueEntry&;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::Queue&;

      private:
        Device::QueueEntry m_entry;

        core::DeferInit<vk::raii::Queue> m_vk_queue;
    };

    class QueueObject: public ParentRef<Queue> {
      public:
        using ParentRef<Queue>::ParentRef;

        [[nodiscard]] auto queue() const noexcept -> const Queue&;
    };

    class CommandPool;

    class CommandPoolObject: public ParentRef<CommandPool> {
      public:
        using ParentRef<CommandPool>::ParentRef;

        [[nodiscard]] auto commandPool() const noexcept -> const CommandPool&;
    };

    struct InheritanceInfo {
        const RenderPass*  render_pass = nullptr;
        core::UInt32       subpass     = 0;
        const FrameBuffer* framebuffer = nullptr;
    };

    class STORMKIT_API CommandBuffer: public DeviceObject,
                                      public QueueObject,
                                      public CommandPoolObject {
        struct Tag {};

      public:
        enum class State {
            Initial,
            Recording,
            Executable
        };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Command_Buffer;

        using Deleter = std::function<void(vk::raii::CommandBuffer&)>;

        ~CommandBuffer();

        CommandBuffer(const CommandBuffer&)                    = delete;
        auto operator=(const CommandBuffer&) -> CommandBuffer& = delete;

        CommandBuffer(CommandBuffer&&) noexcept;
        auto operator=(CommandBuffer&&) noexcept -> CommandBuffer&;

        auto reset() noexcept -> void;
        auto submit(std::span<core::NakedRef<const Semaphore>> wait_semaphores   = {},
                    std::span<core::NakedRef<const Semaphore>> signal_semaphores = {},
                    core::OptionalRef<const Fence> fence = std::nullopt) const noexcept -> void;

        [[nodiscard]] auto state() const noexcept -> State;
        [[nodiscard]] auto level() const noexcept -> CommandBufferLevel;

        auto beginDebugRegion(std::string_view       name,
                              const core::RGBColorF& color = core::RGBColorDef::White<float>)
            -> void;
        auto insertDebugLabel(std::string_view       name,
                              const core::RGBColorF& color = core::RGBColorDef::White<float>)
            -> void;
        auto endDebugRegion() -> void;

        auto begin(bool                           one_time_submit  = false,
                   std::optional<InheritanceInfo> inheritance_info = std::nullopt) -> void;
        auto end() -> void;

        auto beginRenderPass(const RenderPass&           render_pass,
                             const FrameBuffer&          framebuffer,
                             std::span<const ClearValue> clear_values = std::array { ClearValue {
                                 ClearColor { .color = core::RGBColorDef::Silver<float> } } },
                             bool                        secondary_commandbuffers = false) -> void;
        auto nextSubPass() -> void;
        auto endRenderPass() -> void;

        auto bindPipeline(const Pipeline& pipeline) -> void;
        auto setViewport(core::UInt32 first_viewport, std::span<const Viewport> viewports) -> void;
        auto setScissor(core::UInt32 first_scissor, std::span<const Scissor> scissors) -> void;
        auto setLineWidth(float width) -> void;
        auto setDepthBias(float constant_factor, float clamp, float slope_factor) -> void;
        auto setBlendConstants(std::span<const float> constants) -> void;
        auto setDepthBounds(float min, float max) -> void;
        auto setStencilCompareMask(StencilFaceFlag face, core::UInt32 mask) -> void;
        auto setStencilWriteMask(StencilFaceFlag face, core::UInt32 mask) -> void;
        auto setStencilReference(StencilFaceFlag face, core::UInt32 reference) -> void;

        auto dispatch(core::UInt32 group_count_x,
                      core::UInt32 group_count_y,
                      core::UInt32 group_count_z) -> void;

        auto draw(core::UInt32 vertex_count,
                  core::UInt32 instance_count = 1u,
                  core::UInt32 first_vertex   = 0,
                  core::UInt32 first_instance = 0) -> void;
        auto drawIndexed(core::UInt32 index_count,
                         core::UInt32 instance_count = 1u,
                         core::UInt32 first_index    = 0u,
                         core::Int32  vertex_offset  = 0,
                         core::UInt32 first_instance = 0u) -> void;
        auto drawIndirect(const Buffer&     buffer,
                          core::RangeExtent offset,
                          core::UInt32      draw_count,
                          core::UInt32      stride) -> void;
        auto drawIndexedIndirect(const Buffer&     buffer,
                                 core::RangeExtent offset,
                                 core::UInt32      draw_count,
                                 core::UInt32      stride) -> void;

        auto bindVertexBuffers(std::span<const core::NakedRef<const Buffer>> buffers,
                               std::span<const core::UInt64>                 offsets) -> void;
        auto bindIndexBuffer(const Buffer& buffer,
                             core::UInt64  offset        = 0,
                             bool          large_indices = false) -> void;
        auto
            bindDescriptorSets(const Pipeline&                                      pipeline,
                               const PipelineLayout&                                layout,
                               std::span<const core::NakedRef<const DescriptorSet>> descriptor_sets,
                               std::span<const core::UInt32> dynamic_offsets = {}) -> void;

        auto copyBuffer(const Buffer&     src,
                        const Buffer&     dst,
                        core::RangeExtent size,
                        core::UInt64      src_offset = 0u,
                        core::UInt64      dst_offset = 0u) -> void;
        auto copyBufferToImage(const Buffer&                    src,
                               const Image&                     dst,
                               std::span<const BufferImageCopy> buffer_image_copies = {}) -> void;
        auto copyImageToBuffer(const Image&                     src,
                               const Buffer&                    dst,
                               std::span<const BufferImageCopy> buffer_image_copies = {}) -> void;
        auto copyImage(const Image&                  src,
                       const Image&                  dst,
                       ImageLayout                   src_layout,
                       ImageLayout                   dst_layout,
                       const ImageSubresourceLayers& src_subresource_layers,
                       const ImageSubresourceLayers& dst_subresource_layers,
                       const core::math::ExtentU&    extent) -> void;

        auto resolveImage(const Image&                  src,
                          const Image&                  dst,
                          ImageLayout                   src_layout,
                          ImageLayout                   dst_layout,
                          const ImageSubresourceLayers& src_subresource_layers = {},
                          const ImageSubresourceLayers& dst_subresource_layers = {}) -> void;

        auto blitImage(const Image&                src,
                       const Image&                dst,
                       ImageLayout                 src_layout,
                       ImageLayout                 dst_layout,
                       std::span<const BlitRegion> regions,
                       Filter                      filter) -> void;

        auto transitionImageLayout(const Image&                 image,
                                   ImageLayout                  src_layout,
                                   ImageLayout                  dst_layout,
                                   const ImageSubresourceRange& subresource_range = {}) -> void;

        auto executeSubCommandBuffers(std::span<core::NakedRef<const CommandBuffer>> commandbuffers)
            -> void;

        auto pipelineBarrier(PipelineStageFlag                    src_mask,
                             PipelineStageFlag                    dst_mask,
                             DependencyFlag                       dependency,
                             std::span<const MemoryBarrier>       memory_barriers,
                             std::span<const BufferMemoryBarrier> buffer_memory_barriers,
                             std::span<const ImageMemoryBarrier>  image_memory_barriers) -> void;

        auto pushConstants(const PipelineLayout&       pipeline_layout,
                           ShaderStageFlag             stage,
                           std::span<const core::Byte> data,
                           core::UInt32                offset = 0u) -> void;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::CommandBuffer&;

      private:
        CommandBuffer(const CommandPool&, CommandBufferLevel, vk::raii::CommandBuffer&&, Deleter);

        CommandBufferLevel m_level = CommandBufferLevel::Primary;

        vk::raii::CommandBuffer m_vk_command_buffer;

        Deleter m_deleter;

        State m_state = State::Initial;
    };

    class STORMKIT_API CommandPool: public DeviceObject, public QueueObject {
        struct Tag {};

      public:
        CommandPool(const Device& device, const Queue& queue, Tag);
        ~CommandPool();

        CommandPool(const CommandPool&)             = delete;
        auto operator=(const CommandPool&) noexcept = delete;

        CommandPool(CommandPool&&) noexcept;
        auto operator=(CommandPool&&) noexcept -> CommandPool&;

        [[nodiscard]] static auto create(const Device& device, const Queue& queue) noexcept
            -> Expected<CommandPool>;
        [[nodiscard]] static auto allocate(const Device& device, const Queue& queue) noexcept
            -> Expected<std::unique_ptr<CommandPool>>;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::CommandPool&;

      private:
        auto createCommandBuffers(core::RangeExtent count, CommandBufferLevel level) const noexcept
            -> std::vector<vk::raii::CommandBuffer>;
        auto deleteCommandBuffer(vk::raii::CommandBuffer cmb) const noexcept -> void;

        core::DeferInit<vk::raii::CommandPool> m_vk_command_pool;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    namespace {
        constexpr auto old_layout_access_map =
            frozen::make_unordered_map<vk::ImageLayout,
                                       std::pair<vk::AccessFlags, vk::PipelineStageFlags>>(
                { { vk::ImageLayout::eUndefined,
                    { vk::AccessFlagBits {}, vk::PipelineStageFlagBits::eTopOfPipe } },
                  { vk::ImageLayout::ePreinitialized,
                    { vk::AccessFlagBits {}, vk::PipelineStageFlagBits::eTopOfPipe } },
                  { vk::ImageLayout::eGeneral,
                    { vk::AccessFlagBits::eColorAttachmentWrite |
                          vk::AccessFlagBits::eColorAttachmentRead,
                      vk::PipelineStageFlagBits::eColorAttachmentOutput } },
                  { vk::ImageLayout::eColorAttachmentOptimal,
                    { vk::AccessFlagBits::eColorAttachmentWrite |
                          vk::AccessFlagBits::eColorAttachmentRead,
                      vk::PipelineStageFlagBits::eColorAttachmentOutput } },
                  { vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    { vk::AccessFlagBits::eDepthStencilAttachmentRead |
                          vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                      vk::PipelineStageFlagBits::eLateFragmentTests } },
                  { vk::ImageLayout::eDepthStencilReadOnlyOptimal,
                    { vk::AccessFlagBits::eDepthStencilAttachmentRead,
                      vk::PipelineStageFlagBits::eLateFragmentTests } },
                  { vk::ImageLayout::eShaderReadOnlyOptimal,
                    { vk::AccessFlagBits::eInputAttachmentRead,
                      vk::PipelineStageFlagBits::eFragmentShader } },
                  { vk::ImageLayout::eTransferSrcOptimal,
                    { vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer } },
                  { vk::ImageLayout::eTransferDstOptimal,
                    { vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer } },
                  { vk::ImageLayout::ePresentSrcKHR,
                    { vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eTransfer } } });

        constexpr auto new_layout_access_map =
            frozen::make_unordered_map<vk::ImageLayout,
                                       std::pair<vk::AccessFlags, vk::PipelineStageFlags>>(
                { { vk::ImageLayout::eUndefined, { vk::AccessFlagBits {}, {} } },
                  { vk::ImageLayout::ePreinitialized, { vk::AccessFlagBits {}, {} } },
                  { vk::ImageLayout::eGeneral,
                    { vk::AccessFlagBits::eShaderWrite | vk::AccessFlagBits::eShaderRead,
                      vk::PipelineStageFlagBits::eVertexShader } },
                  { vk::ImageLayout::eColorAttachmentOptimal,
                    { vk::AccessFlagBits::eColorAttachmentWrite |
                          vk::AccessFlagBits::eColorAttachmentRead,
                      vk::PipelineStageFlagBits::eColorAttachmentOutput } },
                  { vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    { vk::AccessFlagBits::eDepthStencilAttachmentWrite |
                          vk::AccessFlagBits::eDepthStencilAttachmentRead,
                      vk::PipelineStageFlagBits::eEarlyFragmentTests } },
                  { vk::ImageLayout::eDepthStencilReadOnlyOptimal,
                    { vk::AccessFlagBits::eShaderRead, vk::PipelineStageFlagBits::eVertexInput } },
                  { vk::ImageLayout::eShaderReadOnlyOptimal,
                    { vk::AccessFlagBits::eShaderRead,
                      vk::PipelineStageFlagBits::eFragmentShader } },
                  { vk::ImageLayout::eTransferSrcOptimal,
                    { vk::AccessFlagBits::eTransferRead, vk::PipelineStageFlagBits::eTransfer } },
                  { vk::ImageLayout::eTransferDstOptimal,
                    { vk::AccessFlagBits::eTransferWrite, vk::PipelineStageFlagBits::eTransfer } },
                  { vk::ImageLayout::ePresentSrcKHR,
                    { vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eTransfer } } });
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Queue::Queue(const Device& device, const Device::QueueEntry& entry, Tag)
        : DeviceObject { device }, m_entry { entry } {
        this->device()
            .vkHandle()
            .getQueue(entry.id, 0)
            .transform(core::monadic::set(m_vk_queue))
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Queue::create(const Device&             device,
                                             const Device::QueueEntry& entry) noexcept
        -> Expected<Queue> try {
        return Queue { device, entry, Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Queue::allocate(const Device&             device,
                                               const Device::QueueEntry& entry) noexcept
        -> Expected<std::unique_ptr<Queue>> try {
        return std::make_unique<Queue>(device, entry, Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Queue::waitIdle() const noexcept -> void {
        m_vk_queue->waitIdle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Queue::submit(std::span<core::NakedRef<const CommandBuffer>> command_buffers,
                      std::span<core::NakedRef<const Semaphore>>     wait_semaphores,
                      std::span<core::NakedRef<const Semaphore>>     signal_semaphores,
                      core::OptionalRef<const Fence>                 fence) const noexcept -> void {
        const auto vk_command_buffers =
            command_buffers | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();
        const auto vk_wait_semaphores =
            wait_semaphores | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();
        const auto vk_signal_semaphores = signal_semaphores | std::views::transform(toVkHandle()) |
                                          std::ranges::to<std::vector>();

        const auto submit_infos = std::array { vk::SubmitInfo {}
                                                   .setWaitSemaphores(vk_wait_semaphores)
                                                   .setSignalSemaphores(vk_signal_semaphores)
                                                   .setCommandBuffers(vk_command_buffers) };

        const auto vk_fence =
            core::either(fence,
                         core::monadic::map(toVkHandle(), core::monadic::init<vk::Fence>()),
                         core::monadic::init<vk::Fence>());

        vkHandle().submit(submit_infos, vk_fence);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Queue::entry() const noexcept -> const Device::QueueEntry& {
        return m_entry;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Queue::vkHandle() const noexcept -> const vk::raii::Queue& {
        return m_vk_queue.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    CommandBuffer::CommandBuffer(const CommandPool&        command_pool,
                                 CommandBufferLevel        level,
                                 vk::raii::CommandBuffer&& command_buffer,
                                 Deleter                   deleter)
        : DeviceObject { command_pool.device() }, QueueObject { command_pool.queue() },
          CommandPoolObject { command_pool }, m_level { level },
          m_vk_command_buffer { std::move(command_buffer) }, m_deleter { std::move(deleter) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE CommandBuffer::~CommandBuffer() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE CommandBuffer::CommandBuffer(CommandBuffer&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::operator=(CommandBuffer&&) noexcept
        -> CommandBuffer& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::reset() noexcept -> void {
        vkHandle().reset(vk::CommandBufferResetFlagBits {});
        m_state = State::Initial;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    auto CommandBuffer::submit(std::span<core::NakedRef<const Semaphore>> wait_semaphores,
                               std::span<core::NakedRef<const Semaphore>> signal_semaphores,
                               core::OptionalRef<const Fence> fence) const noexcept -> void {
        auto cmbs = std::array { core::NakedRef<const CommandBuffer> { *this } };

        queue().submit(cmbs, wait_semaphores, signal_semaphores, std::move(fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::state() const noexcept -> State {
        return m_state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::level() const noexcept -> CommandBufferLevel {
        return m_level;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::beginDebugRegion(std::string_view       name,
                                                               const core::RGBColorF& color)
        -> void {
        core::expects(m_state == State::Recording);

        const auto& dispatcher = toRaiiVkHandle()(device()).getDispatcher();
        if (not dispatcher->vkCmdBeginDebugUtilsLabelEXT) return;

        const auto payload =
            vk::DebugUtilsLabelEXT {}.setPLabelName(std::data(name)).setColor(color);

        m_vk_command_buffer.beginDebugUtilsLabelEXT(payload);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::insertDebugLabel(std::string_view       name,
                                                               const core::RGBColorF& color)
        -> void {
        core::expects(m_state == State::Recording);

        const auto& dispatcher = toRaiiVkHandle()(device()).getDispatcher();
        if (not dispatcher->vkCmdInsertDebugUtilsLabelEXT) return;

        const auto payload =
            vk::DebugUtilsLabelEXT {}.setPLabelName(std::data(name)).setColor(color);

        m_vk_command_buffer.insertDebugUtilsLabelEXT(payload);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::endDebugRegion() -> void {
        core::expects(m_state == State::Recording);

        const auto& dispatcher = toRaiiVkHandle()(device()).getDispatcher();
        if (not dispatcher->vkCmdEndDebugUtilsLabelEXT) return;

        m_vk_command_buffer.endDebugUtilsLabelEXT();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::begin(bool                           one_time_submit,
                                                    std::optional<InheritanceInfo> inheritance_info)
        -> void {
        core::expects(m_state == State::Recording);

        const auto vk_inheritance_info = core::either(
            inheritance_info,
            [](auto&& inheritance_info) noexcept {
                return vk::CommandBufferInheritanceInfo {}
                    .setRenderPass(toVkHandle()(inheritance_info.render_pass))
                    .setSubpass(inheritance_info.subpass)
                    .setFramebuffer(toVkHandle()(inheritance_info.framebuffer));
            },
            core::monadic::init<vk::CommandBufferInheritanceInfo>());

        const auto flags = [this, one_time_submit]() {
            auto flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

            if (!one_time_submit) flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
            if (m_level == CommandBufferLevel::Secondary)
                return vk::CommandBufferUsageFlagBits::eRenderPassContinue;

            return flags;
        }();

        const auto begin_info =
            vk::CommandBufferBeginInfo {}.setFlags(flags).setPInheritanceInfo(&vk_inheritance_info);

        m_vk_command_buffer.begin(begin_info);

        m_state = State::Recording;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::end() -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.end();
        m_state = State::Executable;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::beginRenderPass(const RenderPass&           render_pass,
                                       const FrameBuffer&          framebuffer,
                                       std::span<const ClearValue> clear_values,
                                       bool secondary_commandbuffers) -> void {
        core::expects(m_state == State::Recording);

        const auto vk_clear_values =
            clear_values |
            std::views::transform(core::monadic::either(
                [](const ClearColor& color) noexcept -> decltype(auto) {
                    return vk::ClearValue { .color = vk::ClearColorValue {
                                                .float32 =
                                                    color.color.operator std::array<float, 4>() } };
                },
                [](const ClearDepthStencil& depth_stencil) noexcept -> decltype(auto) {
                    return vk::ClearValue { .depthStencil = vk::ClearDepthStencilValue {
                                                .depth   = depth_stencil.depth,
                                                .stencil = depth_stencil.stencil } };
                })) |
            std::ranges::to<std::vector>();

        const auto begin_info =
            vk::RenderPassBeginInfo {}
                .setRenderPass(toVkHandle()(render_pass))
                .setFramebuffer(toVkHandle()(framebuffer))
                .setRenderArea(vk::Rect2D {}.setOffset({ 0, 0 }).setExtent(
                    { framebuffer.extent().width, framebuffer.extent().height }))
                .setClearValues(vk_clear_values);

        const auto subpass_content = secondary_commandbuffers
                                         ? vk::SubpassContents::eSecondaryCommandBuffers
                                         : vk::SubpassContents::eInline;

        m_vk_command_buffer.beginRenderPass(begin_info, subpass_content);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::nextSubPass() -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.nextSubpass(vk::SubpassContents::eInline);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::endRenderPass() -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.endRenderPass();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::bindPipeline(const Pipeline& pipeline) -> void {
        core::expects(m_state == State::Recording);

        const auto bind_point = (pipeline.type() == Pipeline::Type::Raster)
                                    ? vk::PipelineBindPoint::eGraphics
                                    : vk::PipelineBindPoint::eCompute;

        m_vk_command_buffer.bindPipeline(bind_point, toVkHandle()(pipeline));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setViewport(core::UInt32              first_viewport,
                                                          std::span<const Viewport> viewports)
        -> void {
        core::expects(m_state == State::Recording);

        const auto vk_viewports = viewports |
                                  std::views::transform(core::monadic::as<vk::Viewport>()) |
                                  std::ranges::to<std::vector>();

        m_vk_command_buffer.setViewport(first_viewport, vk_viewports);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setScissor(core::UInt32             first_scissor,
                                                         std::span<const Scissor> scissors)
        -> void {
        core::expects(m_state == State::Recording);

        const auto vk_scissors = scissors | std::views::transform(core::monadic::as<vk::Rect2D>()) |
                                 std::ranges::to<std::vector>();

        m_vk_command_buffer.setScissor(first_scissor, vk_scissors);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setLineWidth(float width) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setLineWidth(width);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setDepthBias(float constant_factor,
                                                           float clamp,
                                                           float slope_factor) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setDepthBias(constant_factor, clamp, slope_factor);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setBlendConstants(std::span<const float> constants)
        -> void {
        core::expects(m_state == State::Recording);

        float data[] = { constants[0], constants[1], constants[2], constants[3] };

        m_vk_command_buffer.setBlendConstants(data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setDepthBounds(float min, float max) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setDepthBounds(min, max);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setStencilCompareMask(StencilFaceFlag face,
                                                                    core::UInt32    mask) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setStencilCompareMask(core::narrow<vk::StencilFaceFlagBits>(face),
                                                  mask);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setStencilWriteMask(StencilFaceFlag face,
                                                                  core::UInt32    mask) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setStencilWriteMask(core::narrow<vk::StencilFaceFlagBits>(face), mask);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::setStencilReference(StencilFaceFlag face,
                                                                  core::UInt32 reference) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.setStencilReference(core::narrow<vk::StencilFaceFlagBits>(face),
                                                reference);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::dispatch(core::UInt32 group_count_x,
                                                       core::UInt32 group_count_y,
                                                       core::UInt32 group_count_z) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.dispatch(group_count_x, group_count_y, group_count_z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::draw(core::UInt32 vertex_count,
                                                   core::UInt32 instance_count,
                                                   core::UInt32 first_vertex,
                                                   core::UInt32 first_instance) -> void {
        core::expects(m_state == State::Recording);
        core::expects(vertex_count > 0);

        m_vk_command_buffer.draw(vertex_count, instance_count, first_vertex, first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::drawIndexed(core::UInt32 index_count,
                                                          core::UInt32 instance_count,
                                                          core::UInt32 first_index,
                                                          core::Int32  vertex_offset,
                                                          core::UInt32 first_instance) -> void {
        core::expects(m_state == State::Recording);
        core::expects(index_count > 0);

        m_vk_command_buffer.drawIndexed(index_count,
                                        instance_count,
                                        first_index,
                                        vertex_offset,
                                        first_instance);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::drawIndirect(const Buffer&     buffer,
                                                           core::RangeExtent offset,
                                                           core::UInt32      draw_count,
                                                           core::UInt32      stride) -> void {
        core::expects(m_state == State::Recording);
        core::expects(draw_count > 0);

        m_vk_command_buffer.drawIndirect(toVkHandle()(buffer), offset, draw_count, stride);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::drawIndexedIndirect(const Buffer&     buffer,
                                                                  core::RangeExtent offset,
                                                                  core::UInt32      draw_count,
                                                                  core::UInt32 stride) -> void {
        core::expects(m_state == State::Recording);
        core::expects(draw_count > 0);

        m_vk_command_buffer.drawIndexedIndirect(toVkHandle()(buffer), offset, draw_count, stride);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::bindVertexBuffers(std::span<const core::NakedRef<const Buffer>> buffers,
                                         std::span<const core::UInt64> offsets) -> void {
        core::expects(m_state == State::Recording);
        core::expects(not std::empty(buffers));
        core::expects(std::size(buffers) == std::size(offsets));

        const auto vk_buffers =
            buffers | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();

        m_vk_command_buffer.bindVertexBuffers(0, vk_buffers, offsets);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::bindIndexBuffer(const Buffer& buffer,
                                                              core::UInt64  offset,
                                                              bool          large_indices) -> void {
        core::expects(m_state == State::Recording);

        m_vk_command_buffer.bindIndexBuffer(toVkHandle()(buffer),
                                            offset,
                                            (large_indices) ? vk::IndexType::eUint16
                                                            : vk::IndexType::eUint32);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::bindDescriptorSets(
        const Pipeline&                                      pipeline,
        const PipelineLayout&                                layout,
        std::span<const core::NakedRef<const DescriptorSet>> descriptor_sets,
        std::span<const core::UInt32>                        dynamic_offsets) -> void {
        core::expects(m_state == State::Recording);

        const auto bind_point = (pipeline.type() == Pipeline::Type::Raster)
                                    ? vk::PipelineBindPoint::eGraphics
                                    : vk::PipelineBindPoint::eCompute;

        const auto vk_descriptor_sets =
            descriptor_sets | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();

        m_vk_command_buffer.bindDescriptorSets(bind_point,
                                               toVkHandle()(layout),
                                               0,
                                               vk_descriptor_sets,
                                               dynamic_offsets);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::copyBuffer(const Buffer&     src,
                                                         const Buffer&     dst,
                                                         core::RangeExtent size,
                                                         core::UInt64      src_offset,
                                                         core::UInt64      dst_offset) -> void {
        const auto vk_copy_buffers = std::array {
            vk::BufferCopy { .srcOffset = src_offset, .dstOffset = dst_offset, .size = size }
        };

        m_vk_command_buffer.copyBuffer(toVkHandle()(src), toVkHandle()(dst), vk_copy_buffers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::copyBufferToImage(const Buffer&                    src,
                                         const Image&                     dst,
                                         std::span<const BufferImageCopy> buffer_image_copies)
            -> void {
        core::expects(m_state == State::Recording);

        const auto DEFAULT_COPY =
            std::array { BufferImageCopy { 0, 0, 0, {}, { 0, 0, 0 }, dst.extent() } };

        if (std::empty(buffer_image_copies)) buffer_image_copies = DEFAULT_COPY;

        const auto vk_copy_regions =
            buffer_image_copies | std::views::transform([](auto&& buffer_image_copy) noexcept {
                const auto image_subresource = vk::ImageSubresourceLayers {
                    .aspectMask = core::narrow<vk::ImageAspectFlagBits>(
                        buffer_image_copy.subresource_layers.aspect_mask),
                    .mipLevel       = buffer_image_copy.subresource_layers.mip_level,
                    .baseArrayLayer = buffer_image_copy.subresource_layers.base_array_layer,
                    .layerCount     = buffer_image_copy.subresource_layers.layer_count,
                };

                return vk::BufferImageCopy {
                    .bufferOffset      = buffer_image_copy.buffer_offset,
                    .bufferRowLength   = buffer_image_copy.buffer_row_length,
                    .bufferImageHeight = buffer_image_copy.buffer_image_height,
                    .imageSubresource  = image_subresource,
                    .imageOffset       = as<vk::Offset3D>(buffer_image_copy.offset),
                    .imageExtent       = as<vk::Extent3D>(buffer_image_copy.extent)
                };
            }) |
            std::ranges::to<std::vector>();

        m_vk_command_buffer.copyBufferToImage(toVkHandle()(src),
                                              toVkHandle()(dst),
                                              vk::ImageLayout::eTransferDstOptimal,
                                              vk_copy_regions);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::copyImageToBuffer(const Image&                     src,
                                         const Buffer&                    dst,
                                         std::span<const BufferImageCopy> buffer_image_copies)
            -> void {
        core::expects(m_state == State::Recording);

        const auto DEFAULT_COPY =
            std::array { BufferImageCopy { 0, 0, 0, {}, { 0, 0, 0 }, src.extent() } };

        if (std::empty(buffer_image_copies)) buffer_image_copies = DEFAULT_COPY;

        const auto vk_copy_regions =
            buffer_image_copies | std::views::transform([](auto&& buffer_image_copy) noexcept {
                const auto image_subresource = vk::ImageSubresourceLayers {
                    .aspectMask = core::narrow<vk::ImageAspectFlagBits>(
                        buffer_image_copy.subresource_layers.aspect_mask),
                    .mipLevel       = buffer_image_copy.subresource_layers.mip_level,
                    .baseArrayLayer = buffer_image_copy.subresource_layers.base_array_layer,
                    .layerCount     = buffer_image_copy.subresource_layers.layer_count,
                };

                return vk::BufferImageCopy {
                    .bufferOffset      = buffer_image_copy.buffer_offset,
                    .bufferRowLength   = buffer_image_copy.buffer_row_length,
                    .bufferImageHeight = buffer_image_copy.buffer_image_height,
                    .imageSubresource  = image_subresource,
                    .imageOffset       = as<vk::Offset3D>(buffer_image_copy.offset),
                    .imageExtent       = as<vk::Extent3D>(buffer_image_copy.extent)
                };
            }) |
            std::ranges::to<std::vector>();

        m_vk_command_buffer.copyImageToBuffer(toVkHandle()(src),
                                              vk::ImageLayout::eTransferDstOptimal,
                                              toVkHandle()(dst),
                                              vk_copy_regions);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::copyImage(const Image&                  src,
                                 const Image&                  dst,
                                 ImageLayout                   src_layout,
                                 ImageLayout                   dst_layout,
                                 const ImageSubresourceLayers& src_subresource_layers,
                                 const ImageSubresourceLayers& dst_subresource_layers,
                                 const core::math::ExtentU&    extent) -> void {
        core::expects(m_state == State::Recording);

        const auto vk_src_subresource_layers = vk::ImageSubresourceLayers {
            .aspectMask = core::narrow<vk::ImageAspectFlagBits>(src_subresource_layers.aspect_mask),
            .mipLevel   = src_subresource_layers.mip_level,
            .baseArrayLayer = src_subresource_layers.base_array_layer,
            .layerCount     = src_subresource_layers.layer_count
        };

        const auto vk_dst_subresource_layers = vk::ImageSubresourceLayers {
            .aspectMask = core::narrow<vk::ImageAspectFlagBits>(dst_subresource_layers.aspect_mask),
            .mipLevel   = dst_subresource_layers.mip_level,
            .baseArrayLayer = dst_subresource_layers.base_array_layer,
            .layerCount     = dst_subresource_layers.layer_count
        };

        const auto vk_regions =
            std::array { vk::ImageCopy { .srcSubresource = vk_src_subresource_layers,
                                         .dstSubresource = vk_dst_subresource_layers,
                                         .extent         = as<vk::Extent3D>(extent) } };

        m_vk_command_buffer.copyImage(toVkHandle()(src),
                                      core::narrow<vk::ImageLayout>(src_layout),
                                      toVkHandle()(dst),
                                      core::narrow<vk::ImageLayout>(dst_layout),
                                      vk_regions);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::resolveImage(const Image&                  src,
                                    const Image&                  dst,
                                    ImageLayout                   src_layout,
                                    ImageLayout                   dst_layout,
                                    const ImageSubresourceLayers& src_subresource_layers,
                                    const ImageSubresourceLayers& dst_subresource_layers) -> void {
        core::expects(m_state == State::Recording);

        const auto vk_extent = as<vk::Extent3D>(dst.extent());

        const auto vk_src_subresource_layers = vk::ImageSubresourceLayers {
            .aspectMask = core::narrow<vk::ImageAspectFlagBits>(src_subresource_layers.aspect_mask),
            .mipLevel   = src_subresource_layers.mip_level,
            .baseArrayLayer = src_subresource_layers.base_array_layer,
            .layerCount     = src_subresource_layers.layer_count
        };

        const auto vk_dst_subresource_layers = vk::ImageSubresourceLayers {
            .aspectMask = core::narrow<vk::ImageAspectFlagBits>(dst_subresource_layers.aspect_mask),
            .mipLevel   = dst_subresource_layers.mip_level,
            .baseArrayLayer = dst_subresource_layers.base_array_layer,
            .layerCount     = dst_subresource_layers.layer_count
        };

        const auto vk_regions =
            std::array { vk::ImageResolve { .srcSubresource = vk_src_subresource_layers,
                                            .dstSubresource = vk_dst_subresource_layers,
                                            .extent         = vk_extent } };

        m_vk_command_buffer.resolveImage(toVkHandle()(src),
                                         core::narrow<vk::ImageLayout>(src_layout),
                                         toVkHandle()(dst),
                                         core::narrow<vk::ImageLayout>(dst_layout),
                                         vk_regions);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::blitImage(const Image&                src,
                                                        const Image&                dst,
                                                        ImageLayout                 src_layout,
                                                        ImageLayout                 dst_layout,
                                                        std::span<const BlitRegion> regions,
                                                        Filter filter) -> void {
        core::expects(m_state == State::Recording);

        const auto vk_regions =
            regions | std::views::transform([](auto&& region) noexcept {
                const auto vk_src_subresource_layers = vk::ImageSubresourceLayers {
                    .aspectMask     = core::narrow<vk::ImageAspectFlagBits>(region.src.aspect_mask),
                    .mipLevel       = region.src.mip_level,
                    .baseArrayLayer = region.src.base_array_layer,
                    .layerCount     = region.src.layer_count
                };

                const auto vk_dst_subresource_layers = vk::ImageSubresourceLayers {
                    .aspectMask     = core::narrow<vk::ImageAspectFlagBits>(region.dst.aspect_mask),
                    .mipLevel       = region.dst.mip_level,
                    .baseArrayLayer = region.dst.base_array_layer,
                    .layerCount     = region.dst.layer_count
                };

                return vk::ImageBlit {
                    .srcSubresource = vk_src_subresource_layers,
                    .srcOffsets     = std::array { as<vk::Offset3D>(region.src_offset[0]),
                                               as<vk::Offset3D>(region.src_offset[1]) },
                    .dstSubresource = vk_dst_subresource_layers,
                    .dstOffsets     = std::array { as<vk::Offset3D>(region.dst_offset[0]),
                                               as<vk::Offset3D>(region.dst_offset[1]) },
                };
            }) |
            std::ranges::to<std::vector>();

        m_vk_command_buffer.blitImage(toVkHandle()(src),
                                      core::narrow<vk::ImageLayout>(src_layout),
                                      toVkHandle()(dst),
                                      core::narrow<vk::ImageLayout>(dst_layout),
                                      vk_regions,
                                      core::narrow<vk::Filter>(filter));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::transitionImageLayout(const Image&                 image,
                                             ImageLayout                  src_layout,
                                             ImageLayout                  dst_layout,
                                             const ImageSubresourceRange& subresource_range)
            -> void {
        core::expects(m_state == State::Recording);

        const auto vk_src_layout = core::narrow<vk::ImageLayout>(src_layout);
        const auto vk_dst_layout = core::narrow<vk::ImageLayout>(dst_layout);

        const auto& src_access = old_layout_access_map.find(vk_src_layout);
        const auto& dst_access = new_layout_access_map.find(vk_dst_layout);

        const auto src_stage = src_access->second.second;
        const auto dst_stage = dst_access->second.second;

        const auto vk_subresource_range = vk::ImageSubresourceRange {
            .aspectMask     = core::narrow<vk::ImageAspectFlagBits>(subresource_range.aspect_mask),
            .baseMipLevel   = subresource_range.base_mip_level,
            .levelCount     = subresource_range.level_count,
            .baseArrayLayer = subresource_range.base_array_layer,
            .layerCount     = subresource_range.layer_count,
        };

        const auto barriers =
            std::array { vk::ImageMemoryBarrier { .srcAccessMask       = src_access->second.first,
                                                  .dstAccessMask       = dst_access->second.first,
                                                  .oldLayout           = vk_src_layout,
                                                  .newLayout           = vk_dst_layout,
                                                  .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                                  .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                                  .image               = toVkHandle()(image),
                                                  .subresourceRange    = vk_subresource_range } };

        m_vk_command_buffer.pipelineBarrier(src_stage, dst_stage, {}, {}, {}, barriers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        CommandBuffer::pipelineBarrier(PipelineStageFlag                    src_mask,
                                       PipelineStageFlag                    dst_mask,
                                       DependencyFlag                       dependency,
                                       std::span<const MemoryBarrier>       memory_barriers,
                                       std::span<const BufferMemoryBarrier> buffer_memory_barriers,
                                       std::span<const ImageMemoryBarrier>  image_memory_barriers)
            -> void {
        const auto vk_memory_barriers =
            memory_barriers | std::views::transform([](auto&& barrier) noexcept -> decltype(auto) {
                return vk::MemoryBarrier {
                    .srcAccessMask = core::narrow<vk::AccessFlagBits>(barrier.src),
                    .dstAccessMask = core::narrow<vk::AccessFlagBits>(barrier.dst),
                };
            }) |
            std::ranges::to<std::vector>();

        const auto vk_buffer_memory_barriers =
            buffer_memory_barriers |
            std::views::transform([](auto&& barrier) noexcept -> decltype(auto) {
                return vk::BufferMemoryBarrier {
                    .srcAccessMask       = core::narrow<vk::AccessFlagBits>(barrier.src),
                    .dstAccessMask       = core::narrow<vk::AccessFlagBits>(barrier.dst),
                    .srcQueueFamilyIndex = barrier.src_queue_family_index,
                    .dstQueueFamilyIndex = barrier.dst_queue_family_index,
                    .buffer              = toVkHandle()(barrier.buffer),
                    .offset              = barrier.offset,
                    .size                = barrier.size
                };
            }) |
            std::ranges::to<std::vector>();

        const auto vk_image_memory_barriers =
            image_memory_barriers |
            std::views::transform([](auto&& barrier) noexcept -> decltype(auto) {
                const auto vk_subresource_range = vk::ImageSubresourceRange {
                    .aspectMask = core::narrow<vk::ImageAspectFlagBits>(barrier.range.aspect_mask),
                    .baseMipLevel   = barrier.range.base_mip_level,
                    .levelCount     = barrier.range.level_count,
                    .baseArrayLayer = barrier.range.base_array_layer,
                    .layerCount     = barrier.range.layer_count
                };

                return vk::ImageMemoryBarrier {
                    .srcAccessMask       = core::narrow<vk::AccessFlagBits>(barrier.src),
                    .dstAccessMask       = core::narrow<vk::AccessFlagBits>(barrier.dst),
                    .oldLayout           = core::narrow<vk::ImageLayout>(barrier.old_layout),
                    .newLayout           = core::narrow<vk::ImageLayout>(barrier.new_layout),
                    .srcQueueFamilyIndex = barrier.src_queue_family_index,
                    .dstQueueFamilyIndex = barrier.dst_queue_family_index,
                    .image               = toVkHandle()(barrier.image),
                    .subresourceRange    = vk_subresource_range
                };
            }) |
            std::ranges::to<std::vector>();

        m_vk_command_buffer.pipelineBarrier(core::narrow<vk::PipelineStageFlagBits>(src_mask),
                                            core::narrow<vk::PipelineStageFlagBits>(dst_mask),
                                            core::narrow<vk::DependencyFlagBits>(dependency),
                                            vk_memory_barriers,
                                            vk_buffer_memory_barriers,
                                            vk_image_memory_barriers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::pushConstants(const PipelineLayout& pipeline_layout,
                                                            ShaderStageFlag       stage,
                                                            std::span<const core::Byte> data,
                                                            core::UInt32 offset) -> void {
        core::expects(m_state == State::Recording);
        core::expects(not std::empty(data));

        m_vk_command_buffer.pushConstants<const core::Byte>(toVkHandle()(pipeline_layout),
                                                            core::narrow<vk::ShaderStageFlagBits>(
                                                                stage),
                                                            offset,
                                                            data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::executeSubCommandBuffers(
        std::span<core::NakedRef<const CommandBuffer>> commandbuffers) -> void {
        core::expects(m_state == State::Recording);

        constexpr auto expectsSecondary = [](auto&& cmb) noexcept -> decltype(auto) {
            core::expects(cmb->level() == CommandBufferLevel::Secondary);
            return cmb;
        };

        const auto vk_command_buffers =
            commandbuffers |
            std::views::transform(core::monadic::map(expectsSecondary, toVkHandle())) |
            std::ranges::to<std::vector>();

        m_vk_command_buffer.executeCommands(vk_command_buffers);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandBuffer::vkHandle() const noexcept
        -> const vk::raii::CommandBuffer& {
        return m_vk_command_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandPool::create(const Device& device,
                                                   const Queue&  queue) noexcept
        -> Expected<CommandPool> try {
        return CommandPool { device, queue, Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandPool::allocate(const Device& device,
                                                     const Queue&  queue) noexcept
        -> Expected<std::unique_ptr<CommandPool>> try {
        return std::make_unique<CommandPool>(device, queue, Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto CommandPool::vkHandle() const noexcept
        -> const vk::raii::CommandPool& {
        return m_vk_command_pool;
    }
} // namespace stormkit::gpu
