// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <array>
#include <cfloat>
#include <storm/core/Format.hpp>
#include <chrono>
#include <functional>
#include <iomanip>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <storm/core/Ranges.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    void seed(UInt32 seed) noexcept;

    template<typename T>
    [[nodiscard]] T rand(T &&min, T &&max) noexcept;

    template<typename T, typename V>
    using IfIsFloatingPoint =
        typename std::enable_if_t<std::is_floating_point_v<std::decay_t<std::remove_cv_t<V>>> &&
                                      std::is_floating_point_v<std::decay_t<std::remove_cv_t<T>>>,
                                  bool>;

    template<typename T, typename V, typename = IfIsFloatingPoint<T, V>>
    [[nodiscard]] constexpr auto realIsEqual(T &&a, V &&b) noexcept;

    template<typename T, typename V>
    [[nodiscard]] constexpr T map(V x, V in_min, V in_max, T out_min, T out_max) noexcept;

    template<class T>
    [[nodiscard]] std::intptr_t extractOwnedPtr(T *ptr) noexcept;
} // namespace storm::core

#include "Numerics.inl"
