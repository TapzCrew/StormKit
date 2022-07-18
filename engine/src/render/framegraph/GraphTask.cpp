// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/framegraph/GraphTask.hpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBase::GraphTaskBase(std::string name, Type type, bool cull_imune)
        : m_name { std::move(name) }, m_type { type }, m_cull_imune { cull_imune } {
        static GraphID s_next_id = 0;

        m_id = s_next_id++;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBase::~GraphTaskBase() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    GraphTaskBase::GraphTaskBase(GraphTaskBase &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphTaskBase::operator=(GraphTaskBase &&other) noexcept -> GraphTaskBase & = default;

} // namespace stormkit::engine
