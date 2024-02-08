// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::gpu {
    class CommandBuffer;

    struct VertexBindingDescription {
        core::UInt32    binding;
        core::UInt32    stride;
        VertexInputRate input_rate = VertexInputRate::Vertex;
    };

    struct VertexInputAttributeDescription {
        core::UInt32 location;
        core::UInt32 binding;
        Format       format;
        core::UInt32 offset;
    };

    struct RasterPipelineVertexInputState {
        std::vector<VertexBindingDescription>        binding_descriptions;
        std::vector<VertexInputAttributeDescription> input_attribute_descriptions;
    };

    struct RasterPipelineInputAssemblyState {
        PrimitiveTopology topology                 = PrimitiveTopology::Triangle_List;
        bool              primitive_restart_enable = false;
    };

    struct RasterPipelineViewportState {
        std::vector<Viewport> viewports;
        std::vector<Scissor>  scissors;
    };

    struct RasterPipelineRasterizationState {
        bool         depth_clamp_enable        = false;
        bool         rasterizer_discard_enable = false;
        PolygonMode  polygon_mode              = PolygonMode::Fill;
        float        line_width                = 1;
        CullModeFlag cull_mode                 = CullModeFlag::Back;
        FrontFace    front_face                = FrontFace::Clockwise;
    };

    struct RasterPipelineMultiSampleState {
        bool            sample_shading_enable = false;
        SampleCountFlag rasterization_samples = SampleCountFlag::C1;
    };

    struct RasterPipelineColorBlendAttachmentState {
        ColorComponentFlag color_write_mask = ColorComponentFlag::RGBA;
        bool               blend_enable     = false;

        BlendFactor    src_color_blend_factor = BlendFactor::One;
        BlendFactor    dst_color_blend_factor = BlendFactor::Zero;
        BlendOperation color_blend_operation  = BlendOperation::Add;

        BlendFactor    src_alpha_blend_factor = BlendFactor::One;
        BlendFactor    dst_alpha_blend_factor = BlendFactor::Zero;
        BlendOperation alpha_blend_operation  = BlendOperation::Substract;
    };

    struct RasterPipelineColorBlendState {
        bool                                                 logic_operation_enable = false;
        LogicOperation                                       logic_operation = LogicOperation::Copy;
        std::vector<RasterPipelineColorBlendAttachmentState> attachments;
        std::array<float, 4> blend_constants = { 0.f, 0.f, 0.f, 0.f };
    };

    struct RasterPipelineDynamicState {
        std::vector<DynamicState> dynamics;
    };

    struct RasterPipelineShaderState {
        std::vector<core::NakedRef<const Shader>> shaders;
    };

    struct RasterPipelineDepthStencilState {
        bool depth_test_enable  = false;
        bool depth_write_enable = false;

        CompareOperation depth_compare_op = CompareOperation::Less;

        bool depth_bounds_test_enable = false;

        float min_depth_bounds = 0.f;
        float max_depth_bounds = 1.f;
    };

    struct RasterPipelineLayout {
        std::vector<core::NakedRef<const DescriptorSetLayout>> descriptor_set_layouts;
        std::vector<PushConstantRange>                         push_constant_ranges;
    };

    struct RasterPipelineState {
        RasterPipelineInputAssemblyState input_assembly_state;
        RasterPipelineViewportState      viewport_state;
        RasterPipelineRasterizationState rasterization_state;
        RasterPipelineMultiSampleState   multisample_state;
        RasterPipelineColorBlendState    color_blend_state;
        RasterPipelineDynamicState       dynamic_state;
        RasterPipelineShaderState        shader_state;
        RasterPipelineVertexInputState   vertex_input_state;
        RasterPipelineDepthStencilState  depth_stencil_state;
    };

    class STORMKIT_API PipelineCache: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline_Cache;

        PipelineCache(const Device& device, std::filesystem::path cache_path, Tag);
        ~PipelineCache();

        PipelineCache(const PipelineCache&)                    = delete;
        auto operator=(const PipelineCache&) -> PipelineCache& = delete;

        PipelineCache(PipelineCache&&) noexcept;
        auto operator=(PipelineCache&&) noexcept -> PipelineCache&;

        [[nodiscard]] static auto
                                  create(const Device&         device,
                                         std::filesystem::path cache_path) noexcept -> Expected<PipelineCache>;
        [[nodiscard]] static auto allocate(const Device&         device,
                                           std::filesystem::path cache_path) noexcept
            -> Expected<std::unique_ptr<PipelineCache>>;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::PipelineCache&;

      private:
        auto createNewPipelineCache() -> VulkanExpected<void>;
        auto readPipelineCache() -> VulkanExpected<void>;
        auto saveCache() -> void;

        static constexpr auto MAGIC   = core::UInt32 { 0xDEADBEEF };
        static constexpr auto VERSION = core::UInt32 { 1u };

        struct SerializedCache {
            struct {
                core::UInt32      magic;
                core::RangeExtent data_size;
                core::UInt64      data_hash;
            } guard;

            struct {
                core::UInt32 version;
                core::UInt64 vendor_id;
                core::UInt64 device_id;
            } infos;

            struct {
                std::array<core::UInt8, vk::UuidSize> value;
            } uuid;
        } m_serialized;

        std::filesystem::path m_path;

        core::DeferInit<vk::raii::PipelineCache> m_vk_pipeline_cache;
    };

    class STORMKIT_API PipelineLayout: public DeviceObject {
        struct Tag {};

      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::PipelineLayout;

        PipelineLayout(const Device& device, const RasterPipelineLayout& layout, Tag);
        ~PipelineLayout();

        PipelineLayout(const PipelineLayout&)                    = delete;
        auto operator=(const PipelineLayout&) -> PipelineLayout& = delete;

        PipelineLayout(PipelineLayout&&) noexcept;
        auto operator=(PipelineLayout&&) noexcept -> PipelineLayout&;

        [[nodiscard]] static auto
                                  create(const Device&               device,
                                         const RasterPipelineLayout& layout) noexcept -> Expected<PipelineLayout>;
        [[nodiscard]] static auto allocate(const Device&               device,
                                           const RasterPipelineLayout& layout) noexcept
            -> Expected<std::unique_ptr<PipelineLayout>>;

        [[nodiscard]] auto rasterLayout() -> const RasterPipelineLayout&;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::PipelineLayout&;

      private:
        RasterPipelineLayout                      m_layout;
        core::DeferInit<vk::raii::PipelineLayout> m_vk_pipeline_layout;
    };

    class STORMKIT_API Pipeline: public DeviceObject {
        struct Tag {};

      public:
        enum class Type {
            Raster,
            Compute
        };

        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline;

        Pipeline(const Device&                                      device,
                 const RasterPipelineState&                         state,
                 const PipelineLayout&                              layout,
                 const RenderPass&                                  render_pass,
                 std::optional<core::NakedRef<const PipelineCache>> cache,
                 Tag);
        ~Pipeline();

        Pipeline(const Pipeline&)                    = delete;
        auto operator=(const Pipeline&) -> Pipeline& = delete;

        Pipeline(Pipeline&&) noexcept;
        auto operator=(Pipeline&&) noexcept -> Pipeline&;

        [[nodiscard]] static auto
            create(const Device&                                      device,
                   const RasterPipelineState&                         state,
                   const PipelineLayout&                              layout,
                   const RenderPass&                                  render_pass,
                   std::optional<core::NakedRef<const PipelineCache>> cache) noexcept
            -> Expected<Pipeline>;
        [[nodiscard]] static auto
            allocate(const Device&                                      device,
                     const RasterPipelineState&                         state,
                     const PipelineLayout&                              layout,
                     const RenderPass&                                  render_pass,
                     std::optional<core::NakedRef<const PipelineCache>> cache) noexcept
            -> Expected<std::unique_ptr<Pipeline>>;

        // auto bind(CommandBuffer& commandbuffer) const noexcept -> void;

        [[nodiscard]] auto type() const noexcept -> Type;
        [[nodiscard]] auto rasterState() const noexcept -> const RasterPipelineState&;

        [[nodiscard]] auto vkHandle() const noexcept -> const vk::raii::Pipeline&;

      private:
        auto doInitRasterPipeline(const PipelineLayout&,
                                  const RenderPass&,
                                  std::optional<core::NakedRef<const PipelineCache>>) noexcept
            -> VulkanExpected<void>;

        Type m_type;

        core::DeferInit<vk::raii::Pipeline> m_vk_pipeline;

        std::variant<RasterPipelineState> m_state;
    };
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    PipelineCache::PipelineCache(const Device& device, std::filesystem::path path, Tag)
        : DeviceObject { device }, m_path { std::move(path) } {
        core::either(std::filesystem::exists(m_path),
                     std::bind_front(&PipelineCache::readPipelineCache, this),
                     std::bind_front(&PipelineCache::createNewPipelineCache, this))
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE PipelineCache::~PipelineCache() {
        if (*m_vk_pipeline_cache.get()) [[likely]]
            saveCache();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE PipelineCache::PipelineCache(PipelineCache&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto PipelineCache::create(const Device&         device,
                                                     std::filesystem::path cache_path) noexcept
        -> Expected<PipelineCache> try {
        return PipelineCache { device, std::move(cache_path), Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto PipelineCache::allocate(const Device&         device,
                                                       std::filesystem::path cache_path) noexcept
        -> Expected<std::unique_ptr<PipelineCache>> try {
        return std::make_unique<PipelineCache>(device, std::move(cache_path), Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        PipelineCache::vkHandle() const noexcept -> const vk::raii::PipelineCache& {
        return m_vk_pipeline_cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    PipelineLayout::PipelineLayout(const Device& device, const RasterPipelineLayout& layout, Tag)
        : DeviceObject { device }, m_layout { layout } {
        const auto set_layouts = m_layout.descriptor_set_layouts |
                                 std::views::transform(toVkHandle()) |
                                 std::ranges::to<std::vector>();

        const auto push_constant_ranges =
            m_layout.push_constant_ranges |
            std::views::transform([](auto&& push_constant_range) noexcept {
                return vk::PushConstantRange {}
                    .setStageFlags(
                        core::narrow<vk::ShaderStageFlagBits>(push_constant_range.stages))
                    .setOffset(push_constant_range.offset)
                    .setSize(core::as<core::UInt32>(push_constant_range.size));
            }) |
            std::ranges::to<std::vector>();

        const auto create_info = vk::PipelineLayoutCreateInfo {}
                                     .setSetLayouts(set_layouts)
                                     .setPushConstantRanges(push_constant_ranges);

        this->device()
            .vkHandle()
            .createPipelineLayout(create_info)
            .transform(core::monadic::set(m_vk_pipeline_layout))
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE PipelineLayout::~PipelineLayout() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        PipelineLayout::operator=(PipelineLayout&& other) noexcept -> PipelineLayout& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto PipelineLayout::create(const Device&               device,
                                                      const RasterPipelineLayout& layout) noexcept
        -> Expected<PipelineLayout> try {
        return PipelineLayout { device, layout, Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto PipelineLayout::allocate(const Device&               device,
                                                        const RasterPipelineLayout& layout) noexcept
        -> Expected<std::unique_ptr<PipelineLayout>> try {
        return std::make_unique<PipelineLayout>(device, layout, Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        PipelineLayout::vkHandle() const noexcept -> const vk::raii::PipelineLayout& {
        return m_vk_pipeline_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    Pipeline::Pipeline(const Device&                                      device,
                       const RasterPipelineState&                         state,
                       const PipelineLayout&                              layout,
                       const RenderPass&                                  render_pass,
                       std::optional<core::NakedRef<const PipelineCache>> cache,
                       Tag)
        : DeviceObject { device }, m_state { state } {
        doInitRasterPipeline(layout, render_pass, std::move(cache))
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Pipeline::~Pipeline() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE Pipeline::Pipeline(Pipeline&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Pipeline::create(const Device&                                      device,
                         const RasterPipelineState&                         state,
                         const PipelineLayout&                              layout,
                         const RenderPass&                                  render_pass,
                         std::optional<core::NakedRef<const PipelineCache>> cache) noexcept
        -> Expected<Pipeline> try {
        return Pipeline { device, state, layout, render_pass, std::move(cache), Tag {} };
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Pipeline::allocate(const Device&                                      device,
                           const RasterPipelineState&                         state,
                           const PipelineLayout&                              layout,
                           const RenderPass&                                  render_pass,
                           std::optional<core::NakedRef<const PipelineCache>> cache) noexcept
        -> Expected<std::unique_ptr<Pipeline>> try {
        return std::make_unique<Pipeline>(device,
                                          state,
                                          layout,
                                          render_pass,
                                          std::move(cache),
                                          Tag {});
    } catch (const Result& result) { return std::unexpected(result); }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Pipeline::type() const noexcept -> Type {
        return m_type;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto
        Pipeline::rasterState() const noexcept -> const RasterPipelineState& {
        core::expects(core::is<RasterPipelineState>(m_state));
        return core::as<RasterPipelineState>(m_state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE auto Pipeline::vkHandle() const noexcept -> const vk::raii::Pipeline& {
        return m_vk_pipeline;
    }
} // namespace stormkit::gpu