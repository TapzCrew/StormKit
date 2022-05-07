// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <stormkit/core/PlatformMacro.hpp>

/// \brief `HASH_FUNC` declare hash func and define an operator== for a given `x` type
#define HASH_FUNC(x)                                                                            \
    template<>                                                                                  \
    struct STORMKIT_PUBLIC std::hash<x> {                                                       \
        [[nodiscard]] auto operator()(const x &) const noexcept -> stormkit::core::Hash64;      \
    };                                                                                          \
    template<>                                                                                  \
    struct std::equal_to<x> {                                                                   \
        [[nodiscard]] auto operator()(const x &first, const x &second) const noexcept -> bool { \
            static auto hasher = hash<x> {};                                                    \
            return hasher(first) == hasher(second);                                             \
        }                                                                                       \
    };
