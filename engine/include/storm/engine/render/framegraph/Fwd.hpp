// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Flags.hpp>
#include <storm/core/Memory.hpp>

namespace storm::engine {
    class FrameGraph;
    DECLARE_PTR_AND_REF(FrameGraph);

    struct TextureDescriptor;
    struct BufferDescriptor;

    class FrameGraphResource;
    DECLARE_PTR_AND_REF(FrameGraphResource);

    using FrameGraphResourceID          = core::Handle32<FrameGraphResource>;
    using FrameGraphResourceIDArray     = std::vector<FrameGraphResourceID>;
    using FrameGraphResourceIDSpan      = core::span<FrameGraphResourceID>;
    using FrameGraphResourceIDConstSpan = core::span<const FrameGraphResourceID>;

    namespace details {
        template<typename T>
        concept FrameGraphDescriptorConcept =
            std::same_as<T, TextureDescriptor> || std::same_as<T, BufferDescriptor>;

        class FrameGraphNode;
        DECLARE_PTR_AND_REF(FrameGraphNode);

        using FrameGraphNodeID          = core::Handle32<FrameGraphNode>;
        using FrameGraphNodeIDArray     = std::vector<FrameGraphNodeID>;
        using FrameGraphNodeIDSpan      = core::span<FrameGraphNodeID>;
        using FrameGraphNodeIDConstSpan = core::span<const FrameGraphNodeID>;
    } // namespace details

    template<typename Descriptor>
    class FrameGraphPassNode;

    class FrameGraphBuilder;

    struct FrameGraphStepData;

    enum class QueueFlag : core::UInt64 {
        Graphics        = 1,
        Compute         = 2,
        Transfert       = 4,
        Async_Graphics  = 16,
        Async_Compute   = 32,
        Async_Transfert = 64,
    };
} // namespace storm::engine

FLAG_ENUM(storm::engine::QueueFlag)
