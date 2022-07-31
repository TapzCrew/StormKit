// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <frozen/unordered_map.h>
#include <frozen/unordered_set.h>

#include <stormkit/core/Concepts.hpp>

namespace stormkit::core {
    template<core::HashableType Key, typename Value, std::size_t N>
    [[nodiscard]] constexpr auto makeFrozenMap(std::pair<Key, Value> const (&items)[N]) noexcept;

    template<typename Key,
             typename Value,
             std::size_t N,
             core::HashFuncType<Value> Hasher,
             core::BinaryPredicateType<Value> Equal>
    [[nodiscard]] constexpr auto makeFrozenMap(const std::pair<Key, Value> (&items)[N],
                                               const Hasher &hash = frozen::elsa<Key> {},
                                               const Equal &equal = std::equal_to<Key> {}) noexcept;

    template<core::HashableType Key, typename Value, std::size_t N>
    [[nodiscard]] constexpr auto
        makeFrozenMap(const std::array<std::pair<Key, Value>, N> &items) noexcept;

    template<typename Key,
             typename Value,
             std::size_t N,
             core::HashFuncType<Value> Hasher,
             core::BinaryPredicateType<Value> Equal>
    [[nodiscard]] constexpr auto makeFrozenMap(const std::array<std::pair<Key, Value>, N> &items,
                                               const Hasher &hash = frozen::elsa<Key> {},
                                               const Equal &equal = std::equal_to<Key> {}) noexcept;
} // namespace stormkit::core

#include "FrozenMap.inl"