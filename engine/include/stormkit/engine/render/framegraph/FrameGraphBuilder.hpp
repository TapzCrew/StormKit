// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <future>

#include <stormkit/core/Format.hpp>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/Fwd.hpp>
#include <stormkit/gpu/pipeline/RenderPassDescription.hpp>
#include <stormkit/gpu/resource/Buffer.hpp>
#include <stormkit/gpu/resource/Image.hpp>

#include <stormkit/gpu/pipeline/RenderPassDescription.hpp>

#include <stormkit/engine/EngineObject.hpp>
#include <stormkit/engine/Fwd.hpp>
#include <stormkit/engine/render/framegraph/BakedFrameGraph.hpp>
#include <stormkit/engine/render/framegraph/GraphTask.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC FrameGraphBuilder: public EngineObject {
      public:
        explicit FrameGraphBuilder(Engine &engine);
        ~FrameGraphBuilder();

        FrameGraphBuilder(const FrameGraphBuilder &) = delete;
        auto operator=(const FrameGraphBuilder &) -> FrameGraphBuilder & = delete;

        FrameGraphBuilder(FrameGraphBuilder &&) noexcept;
        auto operator=(FrameGraphBuilder &&) noexcept -> FrameGraphBuilder &;

        template<typename TaskData, typename... Args>
        auto addTask(std::string name, Args &&...args) -> GraphTask<TaskData> &;

        template<ResourceDescriptionType Description, gpu::ResourceType Resource>
        auto setRetainedResource(std::string name, Description &&description, const Resource &image)
            -> GraphResource<Description> &;

        auto setFinalResource(GraphID final) noexcept -> void;

        auto reset() -> void;
        auto bake() -> void;
        auto createFrameGraph(BakedFrameGraph *old = nullptr) -> BakedFrameGraph;
        auto allocateFrameGraph(BakedFrameGraph *old = nullptr) -> BakedFrameGraphOwnedPtr;

        auto hasTask(GraphID id) const noexcept -> bool;
        auto hasTask(std::string_view name) const noexcept -> bool;

        template<typename TaskData>
        auto getTask(std::string_view name) noexcept -> GraphTask<TaskData> &;
        template<typename TaskData>
        auto getTask(std::string_view name) const noexcept -> const GraphTask<TaskData> &;

        template<typename TaskData>
        auto getTask(GraphID id) noexcept -> GraphTask<TaskData> &;
        template<typename TaskData>
        auto getTask(GraphID id) const noexcept -> const GraphTask<TaskData> &;

        auto getTask(std::string_view name) noexcept -> GraphTaskBase &;
        auto getTask(std::string_view name) const noexcept -> const GraphTaskBase &;
        auto getTask(GraphID id) noexcept -> GraphTaskBase &;
        auto getTask(GraphID id) const noexcept -> const GraphTaskBase &;

        auto hasResource(GraphID id) const noexcept -> bool;
        auto hasResource(std::string_view name) const noexcept -> bool;

        template<typename Description>
        auto getResource(std::string_view name) noexcept -> GraphResource<Description> &;
        template<typename Description>
        auto getResource(std::string_view name) const noexcept
            -> const GraphResource<Description> &;

        template<typename Description>
        auto getResource(GraphID id) noexcept -> GraphResource<Description> &;
        template<typename Description>
        auto getResource(GraphID id) const noexcept -> const GraphResource<Description> &;

        auto getResource(std::string_view name) noexcept -> GraphResourceBase &;
        auto getResource(std::string_view name) const noexcept -> const GraphResourceBase &;
        auto getResource(GraphID id) noexcept -> GraphResourceBase &;
        auto getResource(GraphID id) const noexcept -> const GraphResourceBase &;

      private:
        using ResourceVariant = std::variant<const gpu::Image *, const gpu::Buffer *>;

        struct BufferInfo {
            GraphID id;
            gpu::Buffer::CreateInfo create_info;
            std::string_view name;
        };

        struct ImageInfo {
            GraphID id;
            gpu::Image::CreateInfo create_info;
            gpu::ClearValue clear_value;
            std::string_view name;
        };

        struct Pass {
            GraphID id = INVALID_ID;

            std::optional<gpu::RenderPassDescription> description;
            std::string_view name;

            std::vector<BufferInfo> buffers;
            std::vector<ImageInfo> images;
        };

        auto prepareTask(GraphTaskBase &task) noexcept -> void;

        auto cullUnreferencedResources() noexcept -> void;
        auto buildPhysicalDescriptions() noexcept -> void;
        auto buildImagePhysicalDescriptions(const GraphTaskBase &task) noexcept
            -> std::vector<ImageInfo>;
        auto buildBufferPhysicalDescriptions(const GraphTaskBase &task) noexcept
            -> std::vector<BufferInfo>;
        auto buildRenderPassPhysicalDescription(
            const GraphTaskBase &task,
            core::HashMap<GraphID, gpu::ImageLayout> &layouts) noexcept
            -> gpu::RenderPassDescription;
        auto allocatePhysicalResources() -> BakedFrameGraph::Data;

        std::future<void> m_bake_future;
        std::vector<std::unique_ptr<GraphTaskBase>> m_tasks;
        std::vector<std::unique_ptr<GraphResourceBase>> m_resources;
        GraphID m_final_resource = INVALID_ID;

        core::HashMap<GraphID, ResourceVariant> m_retained_resources;

        std::vector<Pass> m_preprocessed_framegraph;

        friend class GraphTaskBuilder;
    };
} // namespace stormkit::engine

#include "FrameGraphBuilder.inl"
