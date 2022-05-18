// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        template<EnumerationType T>
        constexpr auto toUnderlying(T value) -> std::underlying_type_t<T> {
            using Type = T;

            return as<std::underlying_type_t<Type>>(std::forward<T>(value));
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<FlagsType T>
    constexpr auto checkFlag(T value, T flag) noexcept -> bool {
        return (value & flag) == flag;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<FlagsType T>
    constexpr auto nextValue(T value) noexcept -> T {
        using Type = T;

        return as<Type>(details::toUnderlying(std::forward<T>(value)) << 1);
    }
} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator|(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    return stormkit::core::as<Type>(details::toUnderlying(lhs) | details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator&(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    return stormkit::core::as<Type>(details::toUnderlying(lhs) & details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator^(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    return stormkit::core::as<Type>(details::toUnderlying(lhs) ^ details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator~(T lhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    return stormkit::core::as<Type>(~details::toUnderlying(lhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator|=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    lhs = stormkit::core::as<Type>(details::toUnderlying(lhs) | details::toUnderlying(rhs));
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator&=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    lhs = stormkit::core::as<Type>(details::toUnderlying(lhs) & details::toUnderlying(rhs));
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator^=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    using Type        = T;

    lhs = stormkit::core::as<Type>(details::toUnderlying(lhs) ^ details::toUnderlying(rhs));
    return lhs;
}
