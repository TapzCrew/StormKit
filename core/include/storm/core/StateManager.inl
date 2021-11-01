// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<CHILD_OF_STATE_CONCEPT(T), typename... Args>
    auto StateManager::requestPush(Args &&...args) -> void {
        requestPush(std::make_unique<T>(*this, std::forward<Args>(args)...));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<CHILD_OF_STATE_CONCEPT(T), typename... Args>
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
    template<CHILD_OF_STATE_CONCEPT(T)>
    auto StateManager::top() -> T & {
        return static_cast<T &>(top());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<CHILD_OF_STATE_CONCEPT(T)>
    auto StateManager::top() const -> const T & {
        return static_cast<const T &>(top());
    }
} // namespace storm::core
