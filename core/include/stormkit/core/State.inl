// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto State::owner() const noexcept -> const StateManager & { return m_owner.get(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto State::owner() noexcept -> StateManager & { return m_owner.get(); }
} // namespace stormkit::core
