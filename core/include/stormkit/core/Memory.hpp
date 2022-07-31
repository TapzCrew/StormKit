// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <algorithm>
#include <array>

#if defined(__cpp_lib_stacktrace) && __cpp_lib_stacktrace >= 202011L
    #define STORMKIT_HAS_STD_STACKTRACE
    #include <iostream>
    #include <stacktrace>
#else
    #ifdef module
        #undef module
        #define module_back
    #endif

    #include <backward/backward.hpp>

    #ifdef module_back
        #undef module_back
        #define module
    #endif
#endif

#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

#include <stormkit/core/Concepts.hpp>

namespace stormkit::core {

    /// \brief Create an observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns An observer ptr.
    template<PointerType T>
    constexpr auto makeObserver(T& ptr) noexcept;

    /// \brief Create an observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns An observer ptr.
    template<NonPointerType T>
    constexpr auto makeObserver(T& value) noexcept;

    /// \brief Create a const observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns A const observer ptr.
    template<PointerType T>
    constexpr auto makeConstObserver(const T& ptr) noexcept;

    /// \brief Create a const observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns A const observer ptr.
    template<NonPointerType T>
    constexpr auto makeConstObserver(const T& value) noexcept;

    /// TODO FIX DOC
    /// \brief Create an  from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns An observer ptr.
    template<PointerType T>
    constexpr auto makeRef(T& ptr) noexcept;

    /// TODO FIX DOC
    /// \brief Create an observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns An observer ptr.
    template<NonPointerType T>
    constexpr auto makeRef(T& value) noexcept;

    /// TODO FIX DOC
    /// \brief Create a const observer ptr from a pointer value.
    /// \requires `T` shall to be a raw or a smart pointer.
    /// \param ptr The pointer to observe.
    /// \returns A const observer ptr.
    template<PointerType T>
    constexpr auto makeConstRef(const T& ptr) noexcept;

    /// TODO FIX DOC
    /// \brief Create a const observer ptr from a non-pointer value.
    /// \requires `T` shall not to be a raw or a smart pointer.
    /// \param value The value to observe.
    /// \returns A const observer ptr.
    template<NonPointerType T>
    constexpr auto makeConstRef(const T& value) noexcept;

    /// \brief Create a static array of observer ptr.
    ///
    /// Observer pointers are created with `makeObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of observer ptr.
    template<typename... Args>
    constexpr auto makeObserverStaticArray(Args&&...args) noexcept;

    /// \brief Create a static array of const observer ptr.
    ///
    /// Observer pointers are created with `makeConstObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of const observer ptr.
    template<typename... Args>
    constexpr auto makeConstObserverStaticArray(Args&&...args) noexcept;

    /// \brief Create a dynamic array of observer ptr.
    ///
    /// Observer pointers are created with `makeObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of observer ptr.
    template<typename... Args>
    auto makeObserverArray(Args&&...args) noexcept;

    /// \brief Create a dynamic array of const observer ptr.
    ///
    /// Observer pointers are created with `makeConstObserver`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const observer ptr.
    template<typename... Args>
    auto makeConstObserverArray(Args&&...args) noexcept;

    /// \brief Create a static array of reference_wrapper.
    ///
    /// References are created with `std::ref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A static array of reference_wrapper
    template<typename... Args>
    auto makeRefStaticArray(Args&&...args) noexcept;

    /// \brief Create a dynamic array of reference_wrapper.
    ///
    /// References are created with `std::ref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of reference_wrapper
    template<typename... Args>
    auto makeRefArray(Args&&...args) noexcept;

    /// \brief Create a dynamic array of const reference_wrapper.
    ///
    /// References are created with `std::cref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const reference_wrapper.
    template<typename... Args>
    auto makeConstRefStaticArray(Args&&...args) noexcept;

    /// \brief Create a dynamic array of const reference_wrapper.
    ///
    /// References are created with `std::cref`.
    ///
    /// \param args The list of values to observe.
    /// \returns A dynamic array of const reference_wrapper.
    template<typename... Args>
    auto makeConstRefArray(Args&&...args) noexcept;

    /// \brief Convert an array of data to an array of observer ptr of data.
    ///
    /// Observers ptr are created with makeObserverPtr.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of observer ptrs.
    template<std::ranges::range Container>
    auto toObserverArray(Container&& container);

    /// \brief Convert an array of const data to an array of const observer ptr of data.
    ///
    /// Observers ptr are created with makeConstObserverPtr.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of const observer ptrs.
    template<std::ranges::range Container>
    auto toConstObserverArray(Container&& container);

    /// \brief Convert an array of data to an array of reference_wrapper of data.
    ///
    /// Reference_wrappers are created with std::ref.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of reference_wrapper.
    template<std::ranges::range Container>
    auto toRefArray(Container&& container);

    template<std::ranges::range Container>
    auto toArray(Container&& container);

    /// \brief Convert an array of data to an array of const reference_wrapper of data.
    ///
    /// Reference_wrappers are created with std::ref.
    ///
    /// \param container The array to convert.
    /// \returns A dynamic array of const reference_wrapper.
    template<std::ranges::range Container>
    auto toConstRefArray(Container&& container);

    /// \brief Create a static array of `T`
    /// \param first The first value, the array will be an array of this type
    /// \param args The list of values
    /// \requires all values need to be the same type as / convertible to `T`
    /// \return A static array of T
    template<typename T, typename... Args>
    requires((core::Is<T, Args> && ...) ||
             (std::convertible_to<T, Args> &&
              ...)) constexpr auto makeStaticArray(T&& first, Args&&...args) noexcept;

    /// \brief Create a dynamic array of `T`
    /// \param first The first value, the array will be an array of this type
    /// \param args The list of values
    /// \requires all values need to be the same type as `T`
    /// \return A dynamic array of T
    template<typename T, typename... Args>
    requires((core::Is<T, Args> && ...) ||
             (std::convertible_to<T, Args> && ...)) auto makeArray(T&& first, Args&&...args);

    /// \brief Utility to defer allocation on stack of a class member
    template<typename T>
    using DeferredAlloc = std::optional<T>;

    template<SmartPointerType T>
    [[nodiscard]] auto addressOf(const T& ptr) noexcept -> std::intptr_t;

    template<RawPointerType T>
    [[nodiscard]] constexpr auto addressOf(const T& ptr) noexcept -> std::intptr_t;

    template<std::ranges::input_range Range,
             UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate>
    [[nodiscard]] constexpr auto copyIf(Range&& input, Predicate&& predicate) noexcept;

    template<std::ranges::input_range Range,
             std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda>
    [[nodiscard]] constexpr auto transform(Range&& input, Lambda&& lambda) noexcept;

    template<std::ranges::input_range Range,
             UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate,
             std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda>
    [[nodiscard]] constexpr auto
        transformIf(Range&& input, Predicate&& predicate, Lambda&& lambda) noexcept;

    template<
        std::ranges::input_range Range,
        UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate,
        std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda,
        std::output_iterator<
            std::invoke_result_t<Lambda, const typename std::remove_cvref_t<Range>::value_type&>>
            Iterator>
    constexpr auto
        transformIf(Range&& input, Iterator&& it, Predicate&& predicate, Lambda&& lambda) noexcept
        -> void;

    template<typename T, BinaryPredicateType<std::remove_cvref_t<T>> BinaryPredicate>
    [[nodiscard]] constexpr auto binaryToUnary(T&& value, BinaryPredicate&& predicate) noexcept;

    template<typename T>
    [[nodiscard]] constexpr auto toLambda(T&& t);

    auto printStacktrace() noexcept -> void;

    auto setupSignalHandler() noexcept -> void;

    [[nodiscard]] constexpr auto hton(auto value) noexcept -> decltype(auto);

    template<typename T>
    struct ValueReseter {
        ~ValueReseter() {
            if (reset) value = T {};
        }

        T& value;
        bool reset = true;
    };

    template<typename T>
    [[nodiscard]] constexpr auto makeValueReseter(T& value) {
        return ValueReseter<T> { value };
    };
} // namespace stormkit::core

#include "Memory.inl"
