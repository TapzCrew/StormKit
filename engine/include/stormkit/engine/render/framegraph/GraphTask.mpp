// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <stormkit/core/Types.mpp>

#include <stormkit/gpu/Fwd.hpp>

#include <stormkit/engine/render/framegraph/Fwd.hpp>
#include <stormkit/engine/render/framegraph/GraphResource.mpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC GraphTaskBase {
      public:
        enum class Type { Graphics, Compute };

        GraphTaskBase(std::string name, Type type = Type::Graphics, bool cull_imune = false);
        virtual ~GraphTaskBase() = 0;

        GraphTaskBase(const GraphTaskBase &) = delete;
        auto operator=(const GraphTaskBase &) -> GraphTaskBase & = delete;

        GraphTaskBase(GraphTaskBase &&) noexcept;
        auto operator=(GraphTaskBase &&) noexcept -> GraphTaskBase &;

        auto id() const noexcept -> GraphID;
        auto name() const noexcept -> const std::string &;
        auto type() const noexcept -> Type;
        auto cullImune() const noexcept -> bool;
        auto refCount() const noexcept -> core::USize;

        auto creates() const noexcept -> std::span<const GraphID>;
        auto writes() const noexcept -> std::span<const GraphID>;
        auto reads() const noexcept -> std::span<const GraphID>;

        auto setCullImune(bool imune) noexcept -> void;

      protected:
        virtual auto setup(GraphTaskBuilder &builder) -> void                              = 0;
        virtual auto execute(gpu::RenderPass *renderpass, gpu::CommandBuffer &cmb) -> void = 0;

        friend class FrameGraphBuilder; // TODO rework this

      private:
        GraphID m_id;
        std::string m_name;
        Type m_type;
        bool m_cull_imune;
        core::USize m_ref_count = 0;

        std::vector<GraphID> m_creates;
        std::vector<GraphID> m_writes;
        std::vector<GraphID> m_reads;

        friend class GraphTaskBuilder;  // TODO rework this
        friend class FrameGraphBuilder; // TODO rework this
    };

    template<typename T>
    class STORMKIT_PUBLIC GraphTask: public GraphTaskBase {
      public:
        using TaskData      = T;
        using SetupCallback = std::function<void(T &, GraphTaskBuilder &)>;
        using ExecuteCallback =
            std::function<void(const T &, gpu::RenderPass *, gpu::CommandBuffer &)>;

        GraphTask(std::string name,
                  SetupCallback &&setup,
                  ExecuteCallback &&execute,
                  Type type       = Type::Graphics,
                  bool cull_imune = false);
        ~GraphTask() override;

        GraphTask(const GraphTask &) = delete;
        auto operator=(const GraphTask &) -> GraphTask & = delete;

        GraphTask(GraphTask &&) noexcept = default;
        auto operator=(GraphTask &&) noexcept -> GraphTask & = default;

        auto data() const noexcept -> const TaskData &;

      protected:
        auto setup(GraphTaskBuilder &builder) -> void override;
        auto execute(gpu::RenderPass *renderpass, gpu::CommandBuffer &cmb) -> void override;

      private:
        TaskData m_data;

        SetupCallback m_setup;
        ExecuteCallback m_execute;
    };
} // namespace stormkit::engine

#include "GraphTask.inl"
