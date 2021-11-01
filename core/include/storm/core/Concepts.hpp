// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstddef>
#include <cstdint>
#include <functional>

#include <storm/core/Ranges.hpp>

#if __has_include(<concepts>)
    #include <concepts>
    #define STORMKIT_USE_STD_CONCEPTS
#endif

namespace storm::core {
#ifndef STORMKIT_USE_STD_CONCEPTS
    namespace details {
        template<class From, class To>
        concept convertible_to =
            std::is_convertible_v<From, To> &&requires(std::add_rvalue_reference_t<From> (&f)()) {
            static_cast<To>(f());
        };
    } // namespace details
#endif

    template<typename T>
#ifdef STORMKIT_USE_STD_CONCEPTS
    concept ColorComponentType = std::same_as<T, float> || std::same_as<T, std::uint8_t>;
#else
    concept ColorComponentType = std::is_same_v<T, float> || std::is_same_v<T, std::uint8_t>;
#endif
#define COLOR_COMPONENT_CONCEPT(x) storm::core::ColorComponentType x

    template<typename T>
#ifdef STORMKIT_USE_STD_CONCEPTS
    concept EqualityComparable = std::equality_comparable<T>;
#else
    concept EqualityComparable = requires(const std::remove_reference_t<T> &t,
                                          const std::remove_reference_t<T> &u) {
        { t == u }
        ->details::convertible_to<bool>;
        { t != u }
        ->details::convertible_to<bool>;
        { u == t }
        ->details::convertible_to<bool>;
        { u != t }
        ->details::convertible_to<bool>;
    };
#endif
#define EQUALITY_COMPARABLE_CONCEPT(x) storm::core::EqualityComparable x

    template<typename From, typename To>
    concept CastableTo = requires(const std::remove_reference_t<From> &from) {
        { static_cast<To>(from) }
#ifdef STORMKIT_USE_STD_CONCEPTS
        ->std::convertible_to<To>;
#else
        ->details::convertible_to<To>
#endif
    };

    template<typename T>
    concept ByteType = std::is_same_v<T, std::byte> || CastableTo<T, std::byte> ||
#ifdef STORMKIT_USE_STD_CONCEPTS
                       std::convertible_to<T, std::byte>;
#else
                       details::convertible_to<T, std::byte>
#endif
#define BYTE_CONCEPT(x) storm::core::ByteType x
#define BYTE_VARIADIC_CONCEPT(x) storm::core::ByteType... x

    template<typename T>
    concept Enumeration = std::is_enum_v<T>;
#define ENUMERATION_CONCEPT(x) storm::core::Enumeration x

    template<typename T>
    concept Hashable = requires(const T &a) {
        { std::hash<T> {}(a) }
#ifdef STORMKIT_USE_STD_CONCEPTS
        ->std::convertible_to<std::size_t>;
#else
        ->details::convertible_to<std::size_t>
#endif
    };
#define HASHABLE_CONCEPT(x) storm::core::Hashable x

#ifdef STORMKIT_USE_STD_CONCEPTS
    template<typename T>
    concept ArithmeticType = std::integral<T> || std::floating_point<T>;
#else
    template<typename T>
    concept ArithmeticType = std::is_integral_v<T> || std::is_floating_point_v<T>;
#endif

#define ARITHMETIC_TYPE_CONCEPT(x) storm::core::ArithmeticType x

    template<typename T>
    concept isSmartPtr = requires(T &a) {
        a.operator->();
        a.operator*();
        a.get();
    };

    template<typename T>
    concept PointerType = std::is_pointer_v<T> || isSmartPtr<T>;

#define POINTER_CONCEPT(x) storm::core::PointerType x

    template<typename T>
    concept NonPointerType = !PointerType<T>;

#define NON_POINTER_CONCEPT(x) storm::core::NonPointerType x

    template<typename T, typename U>
#ifdef STORMKIT_USE_STD_CONCEPTS
    concept SameType = std::same_as<T, U>;
#else
    concept SameType       = std::is_same_v<T, T>;
#endif

#define SAME_TYPE_CONCEPT(x, y) storm::core::SameType<y> x

    template<typename T>
    concept Range = ranges::range<T>;

#define RANGE_CONCEPT(x) storm::core::Range x

    class State;
    template<typename T>
    concept ChildOfState = std::derived_from<T, State>;

#define CHILD_OF_STATE_CONCEPT(x) storm::core::ChildOfState x

} // namespace storm::core
