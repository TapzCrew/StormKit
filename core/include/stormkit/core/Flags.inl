// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        template<EnumerationType T>
        constexpr auto toUnderlying(T value) {
            return as<std::underlying_type_t<T>>(value);
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<EnumerationType T>
    constexpr auto checkFlag(T value, T flag) noexcept -> bool {
        return (value & flag) == flag;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<FlagsType T>
    constexpr auto nextValue(T value) noexcept -> T {
        return as<T>(details::toUnderlying(value) << 1);
    }
} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator|(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    return stormkit::core::as<T>(details::toUnderlying(lhs) | details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator&(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    return stormkit::core::as<T>(details::toUnderlying(lhs) & details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator^(T lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    return stormkit::core::as<T>(details::toUnderlying(lhs) ^ details::toUnderlying(rhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator~(T lhs) noexcept -> T {
    namespace details = stormkit::core::details;
    return stormkit::core::as<T>(~details::toUnderlying(lhs));
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator|=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    lhs = stormkit::core::as<T>(details::toUnderlying(lhs) | details::toUnderlying(rhs));
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator&=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    lhs = stormkit::core::as<T>(details::toUnderlying(lhs) & details::toUnderlying(rhs));
    return lhs;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::FlagsType T>
constexpr auto operator^=(T &lhs, T rhs) noexcept -> T {
    namespace details = stormkit::core::details;
    lhs = stormkit::core::as<T>(details::toUnderlying(lhs) ^ details::toUnderlying(rhs));
    return lhs;
}
