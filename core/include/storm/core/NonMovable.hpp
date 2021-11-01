// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

namespace storm::core {
    class STORMKIT_PUBLIC NonMovable {
      public:
        constexpr NonMovable() noexcept = default;

        ~NonMovable() noexcept = default;

        NonMovable(NonMovable &) noexcept = default;

        NonMovable &operator=(const NonMovable &) noexcept = default;

        NonMovable(NonMovable &&) = delete;

        NonMovable &operator=(NonMovable &&) = delete;
    };
} // namespace storm::core
