// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/RingBuffer.hpp>
#include <storm/core/Tree.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Texture.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/framegraph/FrameGraphBuilder.hpp>
#include <storm/engine/render/framegraph/FrameGraphNode.hpp>
#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

namespace storm::engine {
    struct FrameGraphStepData {
        render::RenderPassConstPtr render_pass    = nullptr;
        render::FramebufferConstPtr frame_buffer  = nullptr;
        render::GraphicsPipelineConstPtr pipeline = nullptr;
    };

    class STORMKIT_PUBLIC FrameGraph: public core::NonCopyable {
      public:
        struct PhysicalTexture {
            FrameGraphResourceID id;
            render::TextureOwnedPtr texture;
        };
        /*struct PhysicalTextureView {
            FrameGraphTextureID id;
            render::TextureView texture_view;
        };
        struct PhysicalSampler {
            FrameGraphTextureID id;
            render::Sampler sampler;
        };*/
        struct PhysicalBuffer {
            FrameGraphResourceID id;
            render::HardwareBufferOwnedPtr buffer;
        };

        explicit FrameGraph(Engine &engine) noexcept;
        ~FrameGraph();

        FrameGraph(FrameGraph &&) noexcept;
        FrameGraph &operator=(FrameGraph &&) noexcept;

        void setBackbuffer(FrameGraphResourceID id);

        template<details::FrameGraphDescriptorConcept Descriptor>
        [[nodiscard]] FrameGraphResourceID
            setRetainedResource(std::string name,
                                Descriptor descriptor,
                                typename Descriptor::PhysicalResource &resource);

        template<typename PassData>
        FrameGraphPassNode<PassData> &
            addPassNode(std::string name,
                        typename FrameGraphPassNode<PassData>::SetupCallback &&setup_callback,
                        typename FrameGraphPassNode<PassData>::ExecuteCallback &&execute_callback,
                        QueueFlag flags = QueueFlag::Graphics);

        void beginNewFrame(render::Surface::Frame &frame);
        void build();
        void execute();

        [[nodiscard]] const FrameGraphResource &getResource(FrameGraphResourceID id) const noexcept;

        [[nodiscard]] std::span<PhysicalTexture> physicalTextures() noexcept;

        [[nodiscard]] std::span<const PhysicalTexture> physicalTextures() const noexcept;

        [[nodiscard]] std::span<PhysicalBuffer> physicalBuffers() noexcept;

        [[nodiscard]] std::span<const PhysicalBuffer> physicalBuffers() const noexcept;

        [[nodiscard]] render::Texture &getPhysicalTexture(FrameGraphResourceID id) noexcept;

        [[nodiscard]] const render::Texture &
            getPhysicalTexture(FrameGraphResourceID id) const noexcept;

        [[nodiscard]] render::HardwareBuffer &getPhysicalBuffer(FrameGraphResourceID id) noexcept;

        [[nodiscard]] const render::HardwareBuffer &
            getPhysicalBuffer(FrameGraphResourceID id) const noexcept;

        ALLOCATE_HELPERS(FrameGraph)
        ENGINE_GETTERS
      private:
        template<details::FrameGraphDescriptorConcept Descriptor>
        FrameGraphResourceID
            createResource(std::string name, Descriptor descriptor, bool transient);

        template<details::FrameGraphDescriptorConcept Descriptor>
        FrameGraphResourceID createTransientResource(std::string name, Descriptor descriptor);

        [[nodiscard]] FrameGraphResource &getResource(FrameGraphResourceID id) noexcept;

        void cullNodes();
        void buildPhysicalResources();
        void buildRenderPasses();
        std::tuple<render::RenderPassDescription, std::vector<render::ClearValue>>
            buildRenderPass(const details::FrameGraphNode &node);
        void realize(FrameGraphResourceID id, const TextureDescriptor &descriptor);
        void realize(FrameGraphResourceID id, const BufferDescriptor &descriptor);

        void cleanup();

        struct RetainedTexture {
            FrameGraphResourceID descriptor;
            render::TextureRef texture;
        };
        struct RetainedBuffer {
            FrameGraphResourceID descriptor;
            render::HardwareBufferRef buffer;
        };

        struct Graph {
            FrameGraphResourceID next_resource_id  = { 0u };
            details::FrameGraphNodeID next_pass_id = { 0u };

            FrameGraphResourceID backbuffer_id = {};

            details::FrameGraphNodeOwnedPtrArray nodes;
            FrameGraphResourceOwnedPtrArray resources;

            core::HashMap<FrameGraphResourceID, core::ArraySize> resource_link;

            std::vector<render::TextureRef> retained_textures;
            std::vector<render::HardwareBufferRef> retained_buffers;

            core::Hash64 hash;
        };

        struct Barrier {
            render::PipelineStageFlag src_mask;
            render::PipelineStageFlag dst_mask;
            render::DependencyFlag dependency;
            render::MemoryBarriers memory_barriers;
            render::BufferMemoryBarriers buffer_memory_barriers;
            render::ImageMemoryBarriers image_memory_barriers;
        };

        struct Step {
            details::FrameGraphNodeID node;

            render::CommandBufferOwnedPtr cmb;
            render::RenderPassPtr render_pass    = nullptr;
            render::FramebufferPtr frame_buffer  = nullptr;
            render::FencePtr fence               = nullptr;
            render::GraphicsPipelinePtr pipeline = nullptr;

            std::vector<Barrier> barriers;

            std::vector<render::ClearValue> clear_values = std::vector<render::ClearValue> {};

            bool present_step = false;
        };

        EngineRef m_engine;
        render::Surface::Frame *m_current_frame = nullptr;

        Graph m_graph;

        std::vector<PhysicalTexture> m_textures;
        std::vector<render::TextureViewOwnedPtr> m_texture_views;
        // std::vector<render::Sampler> m_samplers;
        std::vector<PhysicalBuffer> m_buffers;
        std::vector<render::FramebufferOwnedPtr> m_frame_buffers;
        std::vector<render::FenceOwnedPtr> m_fences;

        std::vector<Step> m_timeline;

        core::HashMap<core::Hash64, Graph> m_cached_graphs;

        friend struct std::hash<Graph>;
        friend struct std::equal_to<Graph>;
        friend class FrameGraphBuilder;
    };
} // namespace storm::engine

HASH_FUNC(storm::engine::FrameGraph::Graph)

#include "FrameGraph.inl"
#include "FrameGraphBuilder.inl"
