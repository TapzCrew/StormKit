// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/// \brief `FLAG_ENUM` Enable flag operators on an enum class
///
/// This macro enable the semantics of flag on an enum `x`, this will define logical comparison
/// operators
///
/// \param e The enumeration promoted to flag
#define FLAG_ENUM(e)                                            \
    template<>                                                  \
    struct stormkit::core::details::EnableBitmaskOperators<e> { \
        constexpr EnableBitmaskOperators() = default;           \
        static constexpr bool enable       = true;              \
    };
