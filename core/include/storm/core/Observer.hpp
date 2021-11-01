// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <storm/core/Memory.hpp>
#include <string>

namespace storm::core {
    template<typename Event, typename CustomData>
    class Observer {
      public:
        explicit Observer() noexcept = default;

        virtual ~Observer() noexcept = default;

        Observer(Observer &&) noexcept = default;

        Observer &operator=(Observer &&) noexcept = default;

        Observer(const Observer &) noexcept = default;

        Observer &operator=(const Observer &) noexcept = default;

        virtual void onNotified(Event, CustomData &&) = 0;
    };
} // namespace storm::core
