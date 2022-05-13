// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/framegraphs/BakedFrameGraph.mpp>
#include <stormkit/engine/render/framegraphs/FrameGraphBuilder.mpp>
#include <stormkit/engine/render/framegraphs/GraphTaskBuilder.mpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::FrameGraphBuilder() {}

    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::~FrameGraphBuilder() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    FrameGraphBuilder::FrameGraphBuilder(FrameGraphBuilder &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::operator=(FrameGraphBuilder &&other) noexcept
        -> FrameGraphBuilder      & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::bake() -> BakedFrameGraphOwnedPtr {
        auto output = BakedFrameGraphOwnedPtr {};

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto FrameGraphBuilder::prepareTask(GraphTaskBase &task) noexcept -> void {}
} // namespace stormkit::engine
