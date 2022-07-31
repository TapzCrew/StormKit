// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    /// \brief Combine a hash with an other hash
    /// \requires `T` need to have a specialisation of std::hash defined
    /// \param hash the output
    /// \param v the value to hash
    template<HashableType T>
    constexpr auto hashCombine(Hash64 &hash, const T &v) noexcept -> void;
} // namespace stormkit::core

template<stormkit::core::HashableType T>
struct std::hash<std::vector<T>> {
    auto operator()(const std::vector<T> &in) const noexcept -> stormkit::core::Hash64;
};

template<stormkit::core::HashableType T>
struct std::hash<std::span<T>> {
    auto operator()(const std::span<T> &in) const noexcept -> stormkit::core::Hash64;
};

#include "Hash.inl"