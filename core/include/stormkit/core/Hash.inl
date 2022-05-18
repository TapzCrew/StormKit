// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<HashableType T>
    constexpr void hashCombine(Hash64 &hash, const T &v) noexcept {
        const auto hasher = std::hash<T> {};
        hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::HashableType T>
auto std::hash<std::vector<T>>::operator()(const std::vector<T> &in) const noexcept
    -> stormkit::core::Hash64 {
    auto seed = stormkit::core::Hash64 { 0 };

    for (const auto &value : in) stormkit::core::hashCombine(seed, value);

    return seed;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::HashableType T>
auto std::hash<std::span<T>>::operator()(const std::span<T> &in) const noexcept
    -> stormkit::core::Hash64 {
    auto seed = stormkit::core::Hash64 { 0 };

    for (const auto &value : in) stormkit::core::hashCombine(seed, value);

    return seed;
}
