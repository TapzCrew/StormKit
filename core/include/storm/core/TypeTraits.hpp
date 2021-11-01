// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <type_traits>
#include <variant>

namespace storm::core {
    template<class... Ts>
    struct overload: Ts... {
        using Ts::operator()...;
    };

    template<class... Ts>
    overload(Ts...) -> overload<Ts...>;

    struct nonesuch {
        ~nonesuch()                = delete;
        nonesuch(nonesuch const &) = delete;
        void operator=(nonesuch const &) = delete;
    };

    namespace _private {
        template<class Default, class AlwaysVoid, template<class...> class Op, class... Args>
        struct detector {
            using value_t = std::false_type;
            using type    = Default;
        };

        template<class Default, template<class...> class Op, class... Args>
        struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
            using value_t = std::true_type;
            using type    = Op<Args...>;
        };
    } // namespace _private

    template<template<class...> class Op, class... Args>
    using detected_t = typename _private::detector<nonesuch, void, Op, Args...>::type;

    template<class Default, template<class...> class Op, class... Args>
    using detected_or = _private::detector<Default, void, Op, Args...>;

    template<template<class...> class Op, class... Args>
    using is_detected = typename _private::detector<nonesuch, void, Op, Args...>::value_t;

    template<template<class...> class Op, class... Args>
    inline constexpr auto is_detected_v = is_detected<Op, Args...>::value;

    template<class Default, template<class...> class Op, class... Args>
    using detected_or_t = typename detected_or<Default, Op, Args...>::type;

    template<class Expected, template<class...> class Op, class... Args>
    using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

    template<class Expected, template<class...> class Op, class... Args>
    inline constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

    template<class To, template<class...> class Op, class... Args>
    using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

    template<class To, template<class...> class Op, class... Args>
    inline constexpr bool is_detected_convertible_v =
        is_detected_convertible<To, Op, Args...>::value;

    template<typename, typename T>
    struct has_method {
        static_assert(std::integral_constant<T, false>::value,
                      "Second template parameter needs to be of function type.");
    };

    template<typename T>
    using const_iterator_t = typename T::const_iterator;

    template<typename T>
    inline constexpr bool has_const_iterator_v = is_detected_v<const_iterator_t, T>;

    template<typename T>
    using cbegin_t = decltype(std::declval<T>().cbegin());

    template<typename T>
    inline constexpr bool has_cbegin_v = is_detected_v<cbegin_t, T>;

    template<typename T>
    using cend_t = decltype(std::declval<T>().cend());

    template<typename T>
    inline constexpr bool has_cend_v = is_detected_v<cend_t, T>;

    template<typename T>
    struct is_container
        : std::integral_constant<bool,
                                 has_const_iterator_v<T> && has_cbegin_v<T> && has_cend_v<T>> {};

    template<typename T>
    inline constexpr bool is_container_v = is_container<T>::value;

    template<typename T, typename Variant>
    struct is_variant_member;

    template<typename T, typename... Types>
    struct is_variant_member<T, std::variant<Types...>>
        : public std::disjunction<std::is_same<T, Types>...> {};

    template<typename T, typename... Types>
    inline constexpr bool is_variant_member_v = is_variant_member<T, Types...>::value;
} // namespace storm::core
