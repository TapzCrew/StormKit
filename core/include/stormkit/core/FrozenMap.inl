// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::HashableType Key, typename Value, std::size_t N>
    constexpr auto makeFrozenMap(std::pair<Key, Value> const (&items)[N]) noexcept {
        return frozen::make_unordered_map<Key, Value, N>(items);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Key,
             typename Value,
             std::size_t N,
             core::HashFuncType<Value> Hasher,
             core::BinaryPredicateType<Value> Equal>
    constexpr auto makeFrozenMap(const std::pair<Key, Value> (&items)[N],
                                 const Hasher &hash,
                                 const Equal &equal) noexcept {
        return frozen::make_unordered_map<Key, Value, N, Hasher, Equal>(items, hash, equal);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<core::HashableType Key, typename Value, std::size_t N>
    constexpr auto makeFrozenMap(const std::array<std::pair<Key, Value>, N> &items) noexcept {
        return frozen::make_unordered_map<Key, Value, N>(items);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Key,
             typename Value,
             std::size_t N,
             core::HashFuncType<Value> Hasher,
             core::BinaryPredicateType<Value> Equal>
    constexpr auto makeFrozenMap(const std::array<std::pair<Key, Value>, N> &items,
                                 const Hasher &hash,
                                 const Equal &equal) noexcept {
        return frozen::make_unordered_map<Key, Value, N, Hasher, Equal>(items, hash, equal);
    }
} // namespace stormkit::core
