// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <variant>

namespace stormkit::core {
    namespace details {
        template<class T>
        concept BooleanTestableType = std::convertible_to<T, bool>;
    }
    template<class T>
    concept ColorComponentType = std::same_as<T, float> || std::same_as<T, std::uint8_t>;

    template<class T>
    concept ByteType = std::same_as<T, std::byte>;

    template<class T>
    concept NonByteType = !
    ByteType<T>;

    template<class T>
    concept EnumerationType = std::is_enum_v<T> && !
    std::same_as<T, std::byte>;

    template<class T>
    concept HashableType = requires(T& a) {
                               { std::hash<T> {}(a) } -> std::convertible_to<std::size_t>;
                           };

    template<class T>
    concept ArithmeticType = std::integral<T> || std::floating_point<T>;

    template<class T>
    concept SmartPointerType = requires(T& a) {
                                   a.operator->();
                                   a.operator*();
                                   a.get();
                               };

    template<class T>
    concept RawPointerType = std::is_pointer_v<T>;

    template<class T>
    concept PointerType = RawPointerType<T> || SmartPointerType<T>;

    template<class T>
    concept NonPointerType = !
    PointerType<T>;

    template<class T>
    concept PreIncrementableType = requires(T& a) { a.operator--(); };

    template<class T, class U>
    concept Is = std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

    template<class T, class U>
    concept IsNot = !
    Is<T, U>;

    template<class T>
    concept StringLikeType = std::convertible_to<T, std::string_view>;

    template<class T>
    concept ReferenceType = std::is_lvalue_reference_v<T>;

    template<class T>
    concept BooleanTestableType =
        details::BooleanTestableType<T> && requires(T&& t) {
                                               {
                                                   !std::forward<T>(t)
                                                   } -> details::BooleanTestableType;
                                           };

    template<class T, class... Args>
    concept PredicateType = std::predicate<T, Args...>;

    template<class T, class U>
    concept UnaryPredicateType = PredicateType<T, const U&>;

    template<class T, class U>
    concept BinaryPredicateType = PredicateType<T, const U&, const U&>;

    template<class T, class U>
    concept HashFuncType = std::regular_invocable<T, U> &&
                           std::convertible_to<std::invoke_result_t<T, U>, std::uint64_t>;
} // namespace stormkit::core

#if defined(STORMKIT_CXX20_MODULES)
}
#endif
