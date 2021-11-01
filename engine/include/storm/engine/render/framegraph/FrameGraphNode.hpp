// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/pipeline/GraphicsPipelineState.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

namespace storm::engine {
    namespace details {
        class STORMKIT_PUBLIC FrameGraphNode: public core::NonCopyable {
          public:
            FrameGraphNode(std::string name, FrameGraphNodeID id, QueueFlag flags) noexcept;
            virtual ~FrameGraphNode();

            FrameGraphNode(FrameGraphNode &&) noexcept;
            FrameGraphNode &operator=(FrameGraphNode &&) noexcept;

            virtual void setup(FrameGraphBuilder &builder)         = 0;
            virtual void execute(const FrameGraphStepData &step,
                                 render::CommandBuffer &cmb) const = 0;

            void setCullImmune(bool immune) noexcept;
            void setExtent(core::Extentf extent) noexcept;

            void setGraphicsPipeline(render::GraphicsPipelineState state) noexcept;

            [[nodiscard]] std::string_view name() const noexcept;
            [[nodiscard]] FrameGraphNodeID id() const noexcept;
            [[nodiscard]] QueueFlag queueFlags() const noexcept;
            [[nodiscard]] bool isCullImmune() const noexcept;
            [[nodiscard]] const core::Extentf &extent() const noexcept;

            [[nodiscard]] FrameGraphResourceIDConstSpan creates() const noexcept;
            [[nodiscard]] FrameGraphResourceIDConstSpan reads() const noexcept;
            [[nodiscard]] FrameGraphResourceIDConstSpan writes() const noexcept;

            [[nodiscard]] bool hasGraphicsPipeline() const noexcept;
            [[nodiscard]] const render::GraphicsPipelineState &graphicsPipeline() const noexcept;

            ALLOCATE_HELPERS(FrameGraphNode)
          private:
            std::string m_name;
            FrameGraphNodeID m_id;
            QueueFlag m_queue_flags;
            bool m_cull_immune = false;

            FrameGraphResourceIDArray m_creates;
            FrameGraphResourceIDArray m_reads;
            FrameGraphResourceIDArray m_writes;

            render::GraphicsPipelineStateOpt m_graphics_pipeline;

            core::Extentf m_extent;

            core::UInt32 m_ref_count = 0u;

            friend class storm::engine::FrameGraph;
            friend class storm::engine::FrameGraphBuilder;
        };
    } // namespace details

    template<typename PassData>
    class FrameGraphPassNode: public details::FrameGraphNode {
      public:
        using SetupCallback   = std::function<void(PassData &, FrameGraphBuilder &)>;
        using ExecuteCallback = std::function<
            void(const FrameGraphStepData &step, const PassData &, render::CommandBuffer &)>;

        FrameGraphPassNode(std::string name,
                           details::FrameGraphNodeID id,
                           QueueFlag flags,
                           SetupCallback setup_callback,
                           ExecuteCallback execute_callback) noexcept;
        ~FrameGraphPassNode();

        FrameGraphPassNode(FrameGraphPassNode &&) noexcept;
        FrameGraphPassNode &operator=(FrameGraphPassNode &&) noexcept;

        [[nodiscard]] PassData &data() noexcept;
        [[nodiscard]] const PassData &data() const noexcept;

        void setup(FrameGraphBuilder &builder) override;
        void execute(const FrameGraphStepData &step, render::CommandBuffer &cmb) const override;

        ALLOCATE_HELPERS(FrameGraphPassNode)
      private:
        PassData m_data;

        SetupCallback m_setup_callback;
        ExecuteCallback m_execute_callback;

        friend class FrameGraphBuilder;
    };
} // namespace storm::engine

#include "FrameGraphNode.inl"
