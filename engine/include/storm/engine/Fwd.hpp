// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <chrono>

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

/////////// - StormKit::core - ///////////
#include <storm/log/Module.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/render/Fwd.hpp>

#include <storm/engine/data/Fwd.hpp>

namespace storm::engine {
    class Engine;
    DECLARE_PTR_AND_REF(Engine);

    struct NameComponent;

    class Profiler;
    DECLARE_PTR_AND_REF(Profiler);

    class State;
    DECLARE_PTR_AND_REF(State);
} // namespace storm::engine

#define ENGINE_GETTER \
    [[nodiscard]] storm::engine::Engine &engine() noexcept { return m_engine.get(); }
#define ENGINE_CONST_GETTER \
    [[nodiscard]] const storm::engine::Engine &engine() const noexcept { return m_engine.get(); }
#define ENGINE_GETTERS \
    ENGINE_GETTER      \
    ENGINE_CONST_GETTER
