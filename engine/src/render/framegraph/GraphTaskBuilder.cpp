// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/framegraph/GraphTaskBuilder.hpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBuilder::GraphTaskBuilder(GraphTaskBase &task, FrameGraphBuilder &framegraph) noexcept
        : m_task { &task }, m_framegraph { &framegraph } {}

    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBuilder::~GraphTaskBuilder() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBuilder::GraphTaskBuilder(const GraphTaskBuilder &other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphTaskBuilder::operator=(const GraphTaskBuilder &other) noexcept
        -> GraphTaskBuilder      & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBuilder::GraphTaskBuilder(GraphTaskBuilder &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphTaskBuilder::operator=(GraphTaskBuilder &&other) noexcept
        -> GraphTaskBuilder      & = default;
} // namespace stormkit::engine
