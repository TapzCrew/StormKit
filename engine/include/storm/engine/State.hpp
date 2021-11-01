// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Fwd.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Numerics.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/State.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/EntityManager.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC State: public core::State {
      public:
        State(core::StateManager &owner, Engine &engine) noexcept;
        ~State() noexcept override;

        State(State &&) noexcept;
        State &operator=(State &&) noexcept;

        [[nodiscard]] entities::EntityManager &world() noexcept;
        [[nodiscard]] const entities::EntityManager &world() const noexcept;

        ENGINE_GETTERS
      protected:
        entities::EntityManager m_world;

      private:
        EngineRef m_engine;
    };
} // namespace storm::engine

#include "State.inl"
