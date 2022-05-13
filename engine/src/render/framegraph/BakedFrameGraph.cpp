// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/framegraphs/BakedFrameGraph.mpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph() {}

    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::~BakedFrameGraph() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph(BakedFrameGraph &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::operator=(BakedFrameGraph &&other) noexcept
        -> BakedFrameGraph      & = default;

} // namespace stormkit::engine
