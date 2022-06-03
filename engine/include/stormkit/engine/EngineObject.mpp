// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/engine/Fwd.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC EngineObject {
      public:
        explicit EngineObject(Engine &engine) noexcept;
        ~EngineObject() noexcept;

        EngineObject(const EngineObject &) noexcept;
        auto operator=(const EngineObject &) noexcept -> EngineObject &;

        EngineObject(EngineObject &&) noexcept;
        auto operator=(EngineObject &&) noexcept -> EngineObject &;

        [[nodiscard]] auto engine() noexcept -> Engine &;
        [[nodiscard]] auto engine() const noexcept -> const Engine &;

      private:
        Engine *m_engine;
    };
} // namespace stormkit::engine

#include "EngineObject.inl"
