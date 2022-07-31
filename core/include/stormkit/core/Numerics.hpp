// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FormatMacro.hpp>

#include <limits>
#include <numeric>
#include <random>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    auto seed(UInt32 seed) noexcept -> void;

    template<std::floating_point T>
    [[nodiscard]] auto rand(T min, T max) noexcept -> T;

    template<std::integral T>
    [[nodiscard]] auto rand(T min, T max) noexcept -> T;

    template<std::floating_point T>
    [[nodiscard]] constexpr auto isEqual(T a, T b) noexcept -> bool;

    template<std::floating_point T, std::floating_point V>
    [[nodiscard]] constexpr auto isEqual(T a, V b) noexcept -> bool;

    template<std::integral T>
    [[nodiscard]] constexpr auto isEqual(T a, T b) noexcept -> bool;

    template<std::integral T, std::integral V>
    [[nodiscard]] constexpr auto isEqual(T a, V b) noexcept -> bool;

    template<ArithmeticType T, ArithmeticType V>
    [[nodiscard]] constexpr auto map(V x, V in_min, V in_max, T out_min, T out_max) noexcept -> T;

    template<std::integral auto start, std::integral auto stop>
    [[nodiscard]] constexpr auto range() noexcept
        -> std::array<decltype(stop - start), stop - start>;

    template<std::integral T, std::convertible_to<T> U = T>
    [[nodiscard]] constexpr auto range(T stop, U increment = 1) noexcept -> std::vector<T>;

    template<std::integral T, std::convertible_to<T> U = T>
    [[nodiscard]] constexpr auto range(T start, T stop, U increment = 1) noexcept -> std::vector<T>;

    namespace literals {
        [[nodiscard]] constexpr auto operator""_kb(unsigned long long x) noexcept -> core::UInt64;

        [[nodiscard]] constexpr auto operator""_mb(unsigned long long x) noexcept -> core::UInt64;

        [[nodiscard]] constexpr auto operator""_gb(unsigned long long x) noexcept -> core::UInt64;

        [[nodiscard]] constexpr auto operator""_kib(unsigned long long x) noexcept -> core::UInt64;

        [[nodiscard]] constexpr auto operator""_mib(unsigned long long x) noexcept -> core::UInt64;

        [[nodiscard]] constexpr auto operator""_gib(unsigned long long x) noexcept -> core::UInt64;
    } // namespace literals
} // namespace stormkit::core

#include "Numerics.inl"
