// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/engine/render/framegraph/Fwd.hpp>

namespace stormkit::engine {
    struct BufferDescription {
        core::USize size;
    };

    struct ImageDescription {
        core::ExtentU extent;
        gpu::ImageType type;
        gpu::PixelFormat format;

        core::UInt32 layers = 1u;

        std::optional<gpu::ClearValue> clear_value;
    };

    class STORMKIT_PUBLIC GraphResourceBase {
      public:
        GraphResourceBase(std::string name, GraphID creator);
        virtual ~GraphResourceBase() = 0;

        GraphResourceBase(const GraphResourceBase &) = delete;
        auto operator=(const GraphResourceBase &) -> GraphResourceBase & = delete;

        GraphResourceBase(GraphResourceBase &&) noexcept;
        auto operator=(GraphResourceBase &&) noexcept -> GraphResourceBase &;

        auto id() const noexcept -> GraphID;
        auto name() const noexcept -> const std::string &;
        auto transient() const noexcept -> bool;
        auto creator() const noexcept -> GraphID;
        auto writers() const noexcept -> std::span<const GraphID>;
        auto readers() const noexcept -> std::span<const GraphID>;
        auto refCount() const noexcept -> core::USize;

      private:
        GraphID m_id;
        std::string m_name;

        GraphID m_creator;

        std::vector<GraphID> m_writers;
        std::vector<GraphID> m_readers;

        core::USize m_ref_count;

        friend class GraphTaskBuilder;  // TODO rework this
        friend class FrameGraphBuilder; // TODO rework this
    };

    template<ResourceDescriptionType D>
    class STORMKIT_PUBLIC GraphResource: public GraphResourceBase {
      public:
        using Description = D;

        GraphResource(std::string name, GraphID creator, Description &&description);
        ~GraphResource() override;

        GraphResource(const GraphResource &) = delete;
        auto operator=(const GraphResource &) -> GraphResource & = delete;

        GraphResource(GraphResource &&) noexcept;
        auto operator=(GraphResource &&) noexcept -> GraphResource &;

        auto description() const noexcept -> const Description &;

      private:
        Description m_description;
    };

    using GraphBuffer = GraphResource<BufferDescription>;
    using GraphImage  = GraphResource<ImageDescription>;
} // namespace stormkit::engine

#include "GraphResource.inl"
