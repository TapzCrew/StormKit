
// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
#pragma once

#include "AsCast.mpp"

namespace stormkit::core {
    namespace details {
        /////////////////////////////////////
        /////////////////////////////////////
        template<ArithmeticType T, ArithmeticType V>
        constexpr auto maxIsGreater() noexcept {
            return std::numeric_limits<T>::max() > std::numeric_limits<V>::max();
        }

        template<ArithmeticType T, ArithmeticType V>
        using SafeTypeHelperType = std::conditional_t<maxIsGreater<T, V>(), T, V>;
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename... Args>
    constexpr auto is(const std::variant<Args...> &variant) noexcept -> bool {
        return std::holds_alternative<To>(variant);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename... Args>
    constexpr auto as(std::variant<Args...> &variant) noexcept -> To & {
        STORMKIT_CONSTEXPR_EXPECTS(is<To>(variant));

        return std::get<To>(variant);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename... Args>
    constexpr auto as(std::variant<Args...> &&variant) noexcept -> To && {
        STORMKIT_CONSTEXPR_EXPECTS(is<To>(variant));

        return std::get<To>(std::move(variant));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename... Args>
    constexpr auto as(const std::variant<Args...> &variant) noexcept -> const To & {
        STORMKIT_CONSTEXPR_EXPECTS(is<To>(variant));

        return std::get<To>(variant);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, typename... Args>
    constexpr auto as(const std::variant<Args...> &&variant) noexcept -> const To && {
        STORMKIT_CONSTEXPR_EXPECTS(is<To>(variant));

        return std::get<To>(std::move(variant));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::signed_integral To, std::signed_integral From>
    constexpr auto as(From value) noexcept -> To {
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::min() <= value);
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::max() > value);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::unsigned_integral To, std::unsigned_integral From>
    constexpr auto as(From value) noexcept -> To {
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::min() <= value);
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::max() > value);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::signed_integral To, std::unsigned_integral From>
    constexpr auto as(From value) noexcept -> To {
        using type = details::SafeTypeHelperType<To, From>;

        STORMKIT_CONSTEXPR_EXPECTS(static_cast<type>(std::numeric_limits<To>::max()) >
                                   static_cast<type>(value));

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::unsigned_integral To, std::signed_integral From>
    constexpr auto as(From value) noexcept -> To {
        using type = details::SafeTypeHelperType<To, From>;

        STORMKIT_CONSTEXPR_EXPECTS(value >= 0);
        STORMKIT_CONSTEXPR_EXPECTS(static_cast<type>(std::numeric_limits<To>::max()) >
                                   static_cast<type>(value));

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point To, std::floating_point From>
    constexpr auto as(From value) noexcept -> To {
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::min() < value);
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::max() > value);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point To, std::signed_integral From>
    constexpr auto as(From value) noexcept -> To {
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::min() < value);
        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<To>::max() > value);

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point To, std::unsigned_integral From>
    constexpr auto as(From value) noexcept -> To {
        using type = details::SafeTypeHelperType<To, From>;

        STORMKIT_CONSTEXPR_EXPECTS(static_cast<type>(std::numeric_limits<To>::max()) >
                                   static_cast<type>(value));

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::unsigned_integral To, std::floating_point From>
    constexpr auto as(From value) noexcept -> To {
        using Type = details::SafeTypeHelperType<To, From>;

        STORMKIT_CONSTEXPR_EXPECTS(value >= 0);
        STORMKIT_CONSTEXPR_EXPECTS(static_cast<Type>(std::numeric_limits<To>::max()) >
                                   static_cast<Type>(value));

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral To, EnumerationType From>
    requires(!ByteType<To>) constexpr auto as(From value) noexcept -> To {
        return as<To>(static_cast<std::underlying_type_t<From>>(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<EnumerationType To, std::integral From>
    requires(!ByteType<From>) constexpr auto as(From value) noexcept -> To {
        using UnderlyingType = std::underlying_type_t<To>;
        using Type           = details::SafeTypeHelperType<UnderlyingType, From>;

        STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<UnderlyingType>::max() > value);

        if constexpr (std::unsigned_integral<UnderlyingType>) {
            STORMKIT_CONSTEXPR_EXPECTS(value >= 0);
        } else {
            STORMKIT_CONSTEXPR_EXPECTS(std::numeric_limits<UnderlyingType>::min() <
                                       core::as<UnderlyingType>(value));
        }

        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<EnumerationType To, EnumerationType From>
    constexpr auto as(From value) noexcept -> To {
        return static_cast<To>(
            core::as<std::underlying_type_t<To>>(core::as<std::underlying_type_t<From>>(value)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral To, ByteType From>
    constexpr auto as(From value) noexcept -> To {
        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ByteType To, std::integral From>
    constexpr auto as(From value) noexcept -> To {
        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename To, std::convertible_to<To> From>
    requires(!ArithmeticType<To> && !ArithmeticType<From>) constexpr auto as(From &value) noexcept
        -> To {
        return static_cast<To>(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RawPointerType To, RawPointerType From>
    requires(ByteType<std::remove_cvref_t<std::remove_pointer_t<To>>>)
        [[nodiscard]] constexpr auto as(From value) noexcept -> To {
        return reinterpret_cast<To>(value);
    }
} // namespace stormkit::core
