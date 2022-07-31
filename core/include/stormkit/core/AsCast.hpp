// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>

#include <concepts>
#include <iostream>
#include <limits>
#include <type_traits>
#include <variant>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Memory.hpp>

namespace stormkit::core {
    template<typename To, typename... Args>
    [[nodiscard]] constexpr auto is(const std::variant<Args...> &variant) noexcept -> bool;

    template<typename To, ReferenceType From>
    requires std::derived_from<To, std::remove_cvref_t<From>>
    [[nodiscard]] constexpr auto is(From &&value) noexcept -> bool;

    template<typename To, RawPointerType From>
    requires std::derived_from<To, std::remove_pointer_t<From>>
    [[nodiscard]] constexpr auto is(From &&value) noexcept -> bool;

    template<typename To, typename... Args>
    [[nodiscard]] constexpr auto as(std::variant<Args...> &variant) noexcept -> To &;

    template<typename To, typename... Args>
    [[nodiscard]] constexpr auto as(std::variant<Args...> &&variant) noexcept -> To &&;

    template<typename To, typename... Args>
    [[nodiscard]] constexpr auto as(const std::variant<Args...> &variant) noexcept -> const To &;

    template<typename To, typename... Args>
    [[nodiscard]] constexpr auto as(const std::variant<Args...> &&variant) noexcept -> const To &&;
    template<std::signed_integral To, std::signed_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::unsigned_integral To, std::unsigned_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::signed_integral To, std::unsigned_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::unsigned_integral To, std::signed_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::floating_point To, std::floating_point From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::floating_point To, std::signed_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::signed_integral To, std::floating_point From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::floating_point To, std::unsigned_integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::unsigned_integral To, std::floating_point From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::integral To, EnumerationType From>
    requires(!ByteType<To>) [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<EnumerationType To, std::integral From>
    requires(!ByteType<From>) [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<EnumerationType To, EnumerationType From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<std::integral To, ByteType From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<ByteType To, std::integral From>
    [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<ReferenceType To, ReferenceType From>
    requires std::derived_from<std::remove_cvref_t<To>, std::remove_cvref_t<From>>
    [[nodiscard]] constexpr auto as(From &&value) -> To;

    template<RawPointerType To, RawPointerType From>
    requires std::derived_from<std::remove_cvref_t<std::remove_pointer_t<To>>,
                               std::remove_cvref_t<std::remove_pointer_t<From>>>
    [[nodiscard]] constexpr auto as(From &&value) noexcept -> To;

    template<typename To, std::convertible_to<To> From>
    requires(!ArithmeticType<To> && !ArithmeticType<From>)
        [[nodiscard]] constexpr auto as(From &value) noexcept -> To;

    template<RawPointerType To, RawPointerType From>
    requires(ByteType<std::remove_cvref_t<std::remove_pointer_t<To>>>)
        [[nodiscard]] constexpr auto as(From value) noexcept -> To;

    template<typename... Ts>
    struct Overload: Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    Overload(Ts...) -> Overload<Ts...>;
} // namespace stormkit::core

#include "AsCast.inl"