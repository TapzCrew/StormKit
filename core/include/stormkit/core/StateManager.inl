// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "StateManager.mpp"

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<ChildOfStateType T, typename... Args>
    auto StateManager::requestPush(Args &&...args) -> void {
        requestPush(std::make_unique<T>(*this, std::forward<Args>(args)...));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<ChildOfStateType T, typename... Args>
    auto StateManager::requestSet(Args &&...args) -> void {
        requestSet(std::make_unique<T>(*this, std::forward<Args>(args)...));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StateManager::hasPendingRequests() const noexcept -> bool {
        return !std::empty(m_action_queue);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<ChildOfStateType T>
    auto StateManager::top() -> T & {
        return static_cast<T &>(top());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<ChildOfStateType T>
    auto StateManager::top() const -> const T & {
        return static_cast<const T &>(top());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto StateManager::size() const noexcept -> USize { return m_stack.size(); }
} // namespace stormkit::core
