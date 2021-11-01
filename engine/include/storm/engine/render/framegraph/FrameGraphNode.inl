// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::setCullImmune(bool immune) noexcept -> void {
            m_cull_immune = immune;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::setExtent(core::Extentf extent) noexcept -> void {
            m_extent = extent;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto
            FrameGraphNode::setGraphicsPipeline(render::GraphicsPipelineState state) noexcept
            -> void {
            m_graphics_pipeline = std::move(state);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::id() const noexcept -> FrameGraphNodeID { return m_id; }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::name() const noexcept -> std::string_view { return m_name; }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::queueFlags() const noexcept -> QueueFlag {
            return m_queue_flags;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::isCullImmune() const noexcept -> bool { return m_cull_immune; }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::extent() const noexcept -> const core::Extentf & {
            return m_extent;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::creates() const noexcept -> FrameGraphResourceIDConstSpan {
            return m_creates;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::reads() const noexcept -> FrameGraphResourceIDConstSpan {
            return m_reads;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::writes() const noexcept -> FrameGraphResourceIDConstSpan {
            return m_writes;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::hasGraphicsPipeline() const noexcept -> bool {
            return m_graphics_pipeline.has_value();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        inline auto FrameGraphNode::graphicsPipeline() const noexcept
            -> const render::GraphicsPipelineState & {
            return *m_graphics_pipeline;
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    FrameGraphPassNode<PassData>::FrameGraphPassNode(std::string name,
                                                     details::FrameGraphNodeID id,
                                                     QueueFlag flags,
                                                     SetupCallback setup_callback,
                                                     ExecuteCallback execute_callback) noexcept
        : FrameGraphNode { std::move(name), id, flags },
          m_setup_callback { std::move(setup_callback) }, m_execute_callback { std::move(
                                                              execute_callback) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    FrameGraphPassNode<PassData>::~FrameGraphPassNode() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    FrameGraphPassNode<PassData>::FrameGraphPassNode(FrameGraphPassNode &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraphPassNode<PassData>::operator=(FrameGraphPassNode &&) noexcept
        -> FrameGraphPassNode &                = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraphPassNode<PassData>::data() noexcept -> PassData & {
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraphPassNode<PassData>::data() const noexcept -> const PassData & {
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraphPassNode<PassData>::setup(FrameGraphBuilder &builder) -> void {
        m_setup_callback(m_data, builder);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename PassData>
    auto FrameGraphPassNode<PassData>::execute(const FrameGraphStepData &step,
                                               render::CommandBuffer &cmb) const -> void {
        m_execute_callback(step, m_data, cmb);
    }
} // namespace storm::engine
