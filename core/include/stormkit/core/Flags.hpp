// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    namespace details {
        template<stormkit::core::EnumerationType T>
        struct EnableBitmaskOperators {
            constexpr EnableBitmaskOperators() = default;
            static constexpr auto enable       = false;
        };

        template<stormkit::core::EnumerationType T>
        static constexpr auto EnableBitmaskOperatorsEnable = EnableBitmaskOperators<T>::enable;

        template<EnumerationType T>
        constexpr auto toUnderlying(T value) -> std::underlying_type_t<T>;
    } // namespace details

    template<class T>
    concept FlagsType = EnumerationType<T> && details::EnableBitmaskOperatorsEnable<T>;

    /// \brief Check if a flag bit is enabled
    /// \requires `Enum` to be an enumeration promoted as a flag with `FLAG_ENUM` macro
    /// \returns true if the flag big is set and false if not
    template<FlagsType T>
    constexpr auto checkFlag(T value, T flag) noexcept -> bool;

    /// \exclude
    template<FlagsType T>
    constexpr auto nextValue(T value) noexcept -> T;

} // namespace stormkit::core

template<stormkit::core::FlagsType T>
constexpr auto operator|(T lhs, T rhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator&(T lhs, T rhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator^(T lhs, T rhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator~(T lhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator|=(T &lhs, T rhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator&=(T &lhs, T rhs) noexcept -> T;

template<stormkit::core::FlagsType T>
constexpr auto operator^=(T &lhs, T rhs) noexcept -> T;

#include "Flags.inl"
