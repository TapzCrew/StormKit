// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>

namespace storm::core {
    class STORMKIT_PUBLIC NonCopyable {
      public:
        constexpr NonCopyable() noexcept = default;

        ~NonCopyable() noexcept = default;

        NonCopyable(NonCopyable &&) noexcept = default;

        NonCopyable &operator=(NonCopyable &&) noexcept = default;

        NonCopyable(const NonCopyable &) noexcept = delete;

        void operator=(const NonCopyable &) noexcept = delete;
    };
} // namespace storm::core
