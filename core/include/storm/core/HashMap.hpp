// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Concepts.hpp>
#include <storm/core/private/robin_hood.h>

namespace storm::core {
    /// Alias to `robin_hood::unordered_map`
    template<HASHABLE_CONCEPT(Key),
             typename T,
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashMap = robin_hood::unordered_map<Key, T, Hash, KeyEqual>;

    /// Alias to `robin_hood::unordered_set`
    template<HASHABLE_CONCEPT(Key),
             typename Hash     = robin_hood::hash<Key>,
             typename KeyEqual = std::equal_to<Key>>
    using HashSet = robin_hood::unordered_set<Key, Hash, KeyEqual>;
} // namespace storm::core
