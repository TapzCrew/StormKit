// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <functional>
#include <string>
#include <type_traits>

/////////// - StormKit::core - ///////////
#include <storm/core/Span.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    /// \brief Combine a hash with an other hash
    /// \requires `T` need to have a specialisation of std::hash defined
    /// \param hash the output
    /// \param v the value to hash
    template<Hashable T>
    constexpr void hashCombine(core::Hash64 &hash, const T &v) noexcept {
        const auto hasher = std::hash<T> {};
        hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
} // namespace storm::core

namespace std {
    template<storm::core::Hashable T>
    struct hash<std::vector<T>> {
        storm::core::Hash64 operator()(const std::vector<T> &in) const noexcept {
            auto seed = storm::core::Hash64 { 0 };
            for (const auto &value : in) storm::core::hashCombine(seed, value);

            return seed;
        }
    };

    template<typename T>
    struct hash<storm::core::span<T>> {
        storm::core::Hash64 operator()(const storm::core::span<T> &in) const noexcept {
            auto seed = storm::core::Hash64 { 0 };
            for (const auto &value : in) storm::core::hashCombine(seed, value);

            return seed;
        }
    };
} // namespace std

/// \brief `HASH_FUNC` declare hash func and define an operator== for a given `x` type
#define HASH_FUNC(x)                                                                               \
    namespace std {                                                                                \
        template<>                                                                                 \
        struct STORMKIT_PUBLIC hash<x> {                                                           \
            [[nodiscard]] storm::core::Hash64 operator()(const x &) const noexcept;                \
        };                                                                                         \
        template<>                                                                                 \
        struct equal_to<x> {                                                                       \
            [[nodiscard]] inline bool operator()(const x &first, const x &second) const noexcept { \
                static auto hasher = hash<x> {};                                                   \
                return hasher(first) == hasher(second);                                            \
            }                                                                                      \
        };                                                                                         \
    }
