// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto State::world() noexcept -> entities::EntityManager & { return m_world; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto State::world() const noexcept -> const entities::EntityManager & { return m_world; }
} // namespace storm::engine
