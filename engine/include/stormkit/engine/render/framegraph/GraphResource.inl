// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "GraphResource.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::id() const noexcept -> GraphID { return m_id; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::name() const noexcept -> std::string_view { return m_name; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::transient() const noexcept -> bool {
        return m_creator != INVALID_ID;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::creator() const noexcept -> GraphID { return m_creator; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::writers() const noexcept -> std::span<const GraphID> {
        return m_writers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::readers() const noexcept -> std::span<const GraphID> {
        return m_readers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphResourceBase::refCount() const noexcept -> core::USize { return m_ref_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType D>
    GraphResource<D>::GraphResource(std::string name, GraphID creator, Description &&description)
        : GraphResourceBase { std::move(name), creator }, m_description { std::forward<Description>(
                                                              description) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType D>
    GraphResource<D>::~GraphResource() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType D>
    GraphResource<D>::GraphResource(GraphResource &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType D>
    auto GraphResource<D>::operator=(GraphResource &&other) noexcept -> GraphResource & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<ResourceDescriptionType D>
    inline auto GraphResource<D>::description() const noexcept -> const Description & {
        return m_description;
    }
} // namespace stormkit::engine
