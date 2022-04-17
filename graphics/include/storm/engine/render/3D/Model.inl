// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Model::hasMaterial() const noexcept -> bool { return m_has_material; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Model::loaded() const noexcept -> bool { return m_loaded; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Model::path() const noexcept -> const std::filesystem::path & { return m_path; }
} // namespace storm::engine
