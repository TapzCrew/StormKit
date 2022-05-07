// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/framegraph/FrameGraphNode.hpp>
#include <storm/engine/render/framegraph/FrameGraphResource.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC FrameGraphBuilder: public core::NonCopyable {
      public:
        FrameGraphBuilder(FrameGraph &frame_graph, details::FrameGraphNode &frame_node) noexcept;
        ~FrameGraphBuilder() noexcept;

        FrameGraphBuilder(FrameGraphBuilder &&) noexcept;
        FrameGraphBuilder &operator=(FrameGraphBuilder &&) noexcept;

        template<details::FrameGraphDescriptorConcept Descriptor>
        [[nodiscard]] FrameGraphResourceID create(std::string name, Descriptor descriptor);

        [[nodiscard]] FrameGraphResourceID read(FrameGraphResourceID input);

        [[nodiscard]] FrameGraphResourceID write(FrameGraphResourceID input);

      private:
        FrameGraphRef m_frame_graph;

        details::FrameGraphNodeRef m_frame_pass;
    };
} // namespace storm::engine
