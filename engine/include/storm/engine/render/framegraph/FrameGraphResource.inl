// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::id() const noexcept -> FrameGraphResourceID { return m_id; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::name() const noexcept -> std::string_view { return m_name; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::isTransient() const noexcept -> bool { return m_is_transient; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::isCullImune() const noexcept -> bool {
        return m_is_cull_immune;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::type() const noexcept -> Type { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::descriptor() const noexcept -> const Descriptor & {
        return m_descriptor;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::createdBy() const noexcept -> details::FrameGraphNodeID {
        return m_created_by;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::readIn() const noexcept -> details::FrameGraphNodeIDConstSpan {
        return m_read_in;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto FrameGraphResource::writedIn() const noexcept
        -> details::FrameGraphNodeIDConstSpan {
        return m_writed_in;
    }
} // namespace storm::engine
