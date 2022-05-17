// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "GraphTask.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::id() const noexcept -> GraphID { return m_id; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::name() const noexcept -> std::string_view { return m_name; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::type() const noexcept -> Type { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::cullImune() const noexcept -> bool { return m_cull_imune; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::refCount() const noexcept -> core::USize { return m_ref_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::creates() const noexcept -> std::span<const GraphID> {
        return m_creates;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::writes() const noexcept -> std::span<const GraphID> {
        return m_writes;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::reads() const noexcept -> std::span<const GraphID> {
        return m_reads;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphTaskBase::setCullImune(bool imune) noexcept -> void { m_cull_imune = imune; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    GraphTask<TaskData>::GraphTask(std::string name,
                                   SetupCallback &&setup,
                                   ExecuteCallback &&execute,
                                   Type type,
                                   bool cull_imune)
        : GraphTaskBase { std::move(name), type, cull_imune }, m_setup { std::move(setup) },
          m_execute { std::move(execute) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    GraphTask<TaskData>::~GraphTask() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto GraphTask<TaskData>::data() const noexcept -> const TaskData & {
        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto GraphTask<TaskData>::setup(GraphTaskBuilder &builder) -> void {
        m_setup(m_data, builder);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename TaskData>
    auto GraphTask<TaskData>::execute(gpu::CommandBuffer &cmb) -> void {
        m_execute(m_data, cmb);
    }
} // namespace stormkit::engine
