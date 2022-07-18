// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <stormkit/core/Types.hpp>

#include <stormkit/engine/render/framegraph/FrameGraphBuilder.hpp>
#include <stormkit/engine/render/framegraph/Fwd.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC GraphTaskBuilder {
      public:
        GraphTaskBuilder(GraphTaskBase &task, FrameGraphBuilder &framegraph) noexcept;
        ~GraphTaskBuilder() noexcept;

        GraphTaskBuilder(const GraphTaskBuilder &) noexcept;
        auto operator=(const GraphTaskBuilder &) noexcept -> GraphTaskBuilder &;

        GraphTaskBuilder(GraphTaskBuilder &&) noexcept;
        auto operator=(GraphTaskBuilder &&) noexcept -> GraphTaskBuilder &;

        template<ResourceDescriptionType Description>
        auto create(std::string name, Description &&description) -> GraphResource<Description> &;

        template<ResourceDescriptionType Description>
        auto read(GraphResource<Description> &resource) -> GraphResource<Description> &;

        template<ResourceDescriptionType Description>
        auto write(GraphResource<Description> &resource) -> GraphResource<Description> &;

        template<ResourceDescriptionType Description>
        auto readwrite(GraphResource<Description> &resource) -> GraphResource<Description> &;

      private:
        GraphTaskBase *m_task           = nullptr;
        FrameGraphBuilder *m_framegraph = nullptr;
    };
} // namespace stormkit::engine

#include "GraphTaskBuilder.inl"
