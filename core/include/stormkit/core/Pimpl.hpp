// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>

#include <iostream>
#include <memory>
#include <utility>

#include <stormkit/core/Memory.hpp>

namespace stormkit::core {
    template<class T, bool initialize = true>
    class Pimpl {
      public:
        Pimpl() noexcept(!initialize);

        template<class... Args>
        explicit(sizeof...(Args) == 1)
            Pimpl(Args &&...args) noexcept(!initialize ||
                                           std::is_nothrow_constructible_v<T, Args...>);

        template<class... Args>
        auto init(Args &&...args) -> void;

        auto operator->() noexcept -> T *;
        auto operator->() const noexcept -> const T *;

        auto operator*() noexcept -> T &;
        auto operator*() const noexcept -> const T &;

        auto get() noexcept -> T *;
        auto get() const noexcept -> const T *;

        operator bool() const noexcept;

      private:
        std::unique_ptr<T> m_ptr;
    };
} // namespace stormkit::core

#include "Pimpl.inl"