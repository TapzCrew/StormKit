// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <functional>
#include <memory>
#include <queue>
#include <stack>

/////////// - StormKit::core - ///////////
#include <storm/core/Concepts.hpp>
#include <storm/core/Fwd.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

namespace storm::core {
    class STORMKIT_PUBLIC STORMKIT_EBCO StateManager final: public core::NonCopyable {
      public:
        StateManager() noexcept;

        StateManager(StateManager &&manager) noexcept;

        StateManager &operator=(StateManager &&manager) noexcept;

        ~StateManager();

        template<CHILD_OF_STATE_CONCEPT(T), typename... Args>
        void requestPush(Args &&...args);

        void requestPush(StateOwnedPtr &&state);

        template<CHILD_OF_STATE_CONCEPT(T), typename... Args>
        void requestSet(Args &&...args);

        void requestSet(StateOwnedPtr &&state);

        void requestPop();

        [[nodiscard]] bool hasPendingRequests() const noexcept;

        void executeRequests();

        void update(storm::core::Secondf delta);

        void requestClear();

        [[nodiscard]] State &top() noexcept;
        [[nodiscard]] const State &top() const noexcept;

        template<CHILD_OF_STATE_CONCEPT(T)>
        [[nodiscard]] T &top();

        template<CHILD_OF_STATE_CONCEPT(T)>
        [[nodiscard]] const T &top() const;

        inline core::ArraySize size() const noexcept { return m_stack.size(); }

      private:
        struct StateManagerAction {
            enum class Type { push, pop, set, clear } type;
            StateOwnedPtr state; // Null when pop
        };

        std::queue<StateManagerAction> m_action_queue;
        std::stack<StateOwnedPtr> m_stack;
    };
} // namespace storm::core

#include "StateManager.inl"
