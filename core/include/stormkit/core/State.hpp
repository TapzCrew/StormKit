// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <stack>
#include <vector>

#include <stormkit/core/Fwd.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    class STORMKIT_PUBLIC State {
      public:
        explicit State(StateManager &owner) noexcept;
        virtual ~State() noexcept;

        State(const State &state) = delete;
        auto operator=(const State &state) -> State & = delete;

        State(State &&state) noexcept;
        auto operator=(State &&state) noexcept -> State &;

        virtual auto pause() -> void = 0;

        virtual auto resume() -> void = 0;

        virtual auto update(core::Secondf delta) -> void = 0;

        auto owner() const noexcept -> const StateManager &;

      protected:
        auto owner() noexcept -> StateManager &;

      private:
        StateManagerRef m_owner;
    };
    DECLARE_PTR_AND_REF(State)

    template<typename T>
    concept ChildOfStateType = std::derived_from<T, State>;
} // namespace stormkit::core

#include "State.inl"