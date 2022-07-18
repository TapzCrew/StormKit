// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <robin_hood.h>

#include <functional>

#include <stormkit/core/Concepts.hpp>

namespace stormkit::core {
    /// Alias to `robin_hood::unordered_map`
    template<HashableType Key,
             typename T,
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashMap = robin_hood::unordered_map<Key, T, Hash, KeyEqual>;

    /// Alias to `robin_hood::unordered_set`
    template<HashableType Key,
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashSet = robin_hood::unordered_set<Key, Hash, KeyEqual>;
} // namespace stormkit::core
