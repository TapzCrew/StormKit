// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::name() const noexcept -> std::string_view { return m_name; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::subDrawables() const noexcept -> core::span<const SubDrawable> {
        return m_sub_drawables;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::boundingBox() const noexcept -> const BoundingBox & {
        return m_bounding_box;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Drawable::dirty() const noexcept -> bool { return m_dirty; }
} // namespace storm::engine
