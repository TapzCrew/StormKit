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
    template<typename D>
    GraphResource<D>::GraphResource(std::string name, GraphID id, Description &&description)
        : GraphResourceBase { std::move(name), id }, m_description { std::forward<Description>(
                                                         description) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename D>
    GraphResource<D>::~GraphResource() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename D>
    GraphResource<D>::GraphResource(GraphResource &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename D>
    auto GraphResource<D>::operator=(GraphResource &&other) noexcept -> GraphResource & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename D>
    inline auto GraphResource<D>::description() const noexcept -> const Description & {
        return m_description;
    }
} // namespace stormkit::engine
