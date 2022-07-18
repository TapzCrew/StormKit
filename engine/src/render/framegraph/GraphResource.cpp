
// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/render/framegraph/GraphResource.hpp>

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    GraphResourceBase::GraphResourceBase(std::string name, GraphID creator)
        : m_creator { creator }, m_name { std::move(name) } {
        static GraphID s_next_id = 0;

        m_id = s_next_id++;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    GraphResourceBase::~GraphResourceBase() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    GraphResourceBase::GraphResourceBase(GraphResourceBase &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphResourceBase::operator=(GraphResourceBase &&other) noexcept
        -> GraphResourceBase      & = default;

} // namespace stormkit::engine
