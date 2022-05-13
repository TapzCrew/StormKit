// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/Types.mpp>

namespace stormkit::engine {
    class BakedFrameGraph;
    DECLARE_PTR_AND_REF(BakedFrameGraph);

    class FrameGraphBuilder;
    DECLARE_PTR_AND_REF(FrameGraphBuilder);

    class GraphResourceBase;
    DECLARE_PTR_AND_REF(GraphResourceBase);

    template<typename Description>
    class GraphResource;
    DECLARE_PTR_AND_REF_TEMPLATE(GraphResource, Description);

    class GraphTaskBase;
    DECLARE_PTR_AND_REF(GraphTaskBase);

    template<typename TaskData>
    class GraphTask;
    DECLARE_PTR_AND_REF_TEMPLATE(GraphTask, TaskData);

    class GraphTaskBuilder;
    DECLARE_PTR_AND_REF(GraphTaskBuilder);

    using GraphID = core::UInt64;
} // namespace stormkit::engine
