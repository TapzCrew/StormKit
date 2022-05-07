// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <type_traits>

/////////// - StormKit::core - ///////////
#include <storm/core/Concepts.hpp>
#include <storm/core/private/Flags.hpp>

/// \brief `FLAG_ENUM` Enable flag operators on an enum class
///
/// This macro enable the semantics of flag on an enum `x`, this will define logical comparison
/// operators
///
/// \param e The enumeration promoted to flag
#define FLAG_ENUM(e)                                    \
    template<>                                          \
    struct enable_bitmask_operators<e> {                \
        constexpr enable_bitmask_operators() = default; \
        static const bool enable             = true;    \
    };

namespace storm::core {
    /// \brief Check if a flag bit is enabled
    /// \requires `Enum` to be an enumeration promoted as a flag with `FLAG_ENUM` macro
    /// \returns true if the flag big is set and false if not
    template<ENUMERATION_CONCEPT(Enum)>
    constexpr bool checkFlag(Enum value, Enum flag) noexcept {
        return (value & flag) == flag;
    }

    /// \exclude
    template<ENUMERATION_CONCEPT(E)>
    constexpr std::enable_if_t<enable_bitmask_operators<E>::enable, E> nextValue(E value) noexcept {
        using underlying_type = std::underlying_type_t<E>;

        return static_cast<E>(static_cast<underlying_type>(value) << 1);
    }
} // namespace storm::core
