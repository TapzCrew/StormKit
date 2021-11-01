// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Memory.hpp"

namespace storm::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<POINTER_CONCEPT(T)>
    constexpr auto makeObserver(T &ptr) noexcept {
        return std::addressof(*ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeObserver(T &value) noexcept {
        return std::addressof(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<POINTER_CONCEPT(T)>
    constexpr auto makeConstObserver(const T &ptr) noexcept {
        return std::addressof(std::as_const(*ptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeConstObserver(const T &value) noexcept {
        return std::addressof(std::as_const(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<POINTER_CONCEPT(T)>
    constexpr auto makeRef(T &ptr) noexcept {
        return std::ref(*ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeRef(T &value) noexcept {
        return std::ref(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<POINTER_CONCEPT(T)>
    constexpr auto makeConstRef(const T &ptr) noexcept {
        return std::cref(std::as_const(*ptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NON_POINTER_CONCEPT(T)>
    constexpr auto makeConstRef(const T &value) noexcept {
        return std::cref(std::as_const(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    constexpr auto makeObserverStaticArray(Args &&...args) noexcept {
        return makeStaticArray(makeObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    constexpr auto makeConstObserverStaticArray(Args &&...args) noexcept {
        return makeStaticArray(makeConstObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeObserverArray(Args &&...args) noexcept {
        return makeArray(makeObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstObserverArray(Args &&...args) noexcept {
        return makeArray(makeConstObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeRefStaticArray(Args &&...args) noexcept {
        return makeStaticArray(makeRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstRefStaticArray(Args &&...args) noexcept {
        return makeStaticArray(makeConstRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeRefArray(Args &&...args) noexcept {
        return makeArray(makeRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstRefArray(Args &&...args) noexcept {
        return makeArray(makeConstRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RANGE_CONCEPT(Container)>
    auto toObserverArray(Container &&container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<value_type *> {};
        vec.reserve(std::size(container));

        ranges::transform(container, std::back_inserter(vec), makeObserver);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RANGE_CONCEPT(Container)>
    auto toConstObserverArray(Container &&container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<const value_type *> {};
        vec.reserve(std::size(container));

        ranges::transform(container, std::back_inserter(vec), makeConstObserver);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RANGE_CONCEPT(Container)>
    auto toRefArray(Container &&container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<std::reference_wrapper<value_type>> {};
        vec.reserve(std::size(container));

        ranges::transform(container, std::back_inserter(vec), std::ref);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RANGE_CONCEPT(Container)>
    auto toConstRefArray(Container &&container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<const value_type &> {};
        vec.reserve(std::size(container));

        ranges::transform(container, std::back_inserter(vec), std::cref);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, SAME_TYPE_CONCEPT(... Args, T)>
    constexpr auto makeStaticArray(T &&first, Args &&...args) noexcept {
        return std::array { std::forward<T>(first), std::forward<Args>(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, SAME_TYPE_CONCEPT(... Args, T)>
    auto makeArray(T &&first, Args &&...args) noexcept {
        return std::vector { std::forward<T>(first), std::forward<Args>(args)... };
    }
} // namespace storm::core
