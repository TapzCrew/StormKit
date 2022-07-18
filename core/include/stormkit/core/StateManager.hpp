// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <stack>
#include <vector>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Fwd.hpp>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/State.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    class STORMKIT_PUBLIC StateManager {
      public:
        StateManager() noexcept;

        StateManager(const StateManager &manager) noexcept = delete;
        auto operator=(const StateManager &manager) noexcept -> StateManager & = delete;

        StateManager(StateManager &&manager) noexcept;
        auto operator=(StateManager &&manager) noexcept -> StateManager &;

        ~StateManager();

        template<ChildOfStateType T, typename... Args>
        auto requestPush(Args &&...args) -> void;

        auto requestPush(StateOwnedPtr &&state) -> void;

        template<ChildOfStateType T, typename... Args>
        auto requestSet(Args &&...args) -> void;

        auto requestSet(StateOwnedPtr &&state) -> void;

        auto requestPop() -> void;

        [[nodiscard]] auto hasPendingRequests() const noexcept -> bool;

        auto executeRequests() -> void;

        auto update(Secondf delta) -> void;

        auto requestClear() -> void;

        [[nodiscard]] auto top() noexcept -> State &;
        [[nodiscard]] auto top() const noexcept -> const State &;

        template<ChildOfStateType T>
        [[nodiscard]] auto top() -> T &;

        template<ChildOfStateType T>
        [[nodiscard]] auto top() const -> const T &;

        auto size() const noexcept -> USize;

      private:
        struct StateManagerAction {
            enum class Type { push, pop, set, clear } type;
            StateOwnedPtr state; // Null when pop
        };

        std::queue<StateManagerAction> m_action_queue;
        std::stack<StateOwnedPtr> m_stack;
    };
} // namespace stormkit::core

#include "StateManager.inl"
