// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Memory.hpp"

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType T>
    constexpr auto makeObserver(T& ptr) noexcept {
        return std::addressof(*ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    constexpr auto makeObserver(T& value) noexcept {
        return std::addressof(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType T>
    constexpr auto makeConstObserver(const T& ptr) noexcept {
        return std::addressof(std::as_const(*ptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    constexpr auto makeConstObserver(const T& value) noexcept {
        return std::addressof(std::as_const(value));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType T>
    constexpr auto makeRef(T& ptr) noexcept {
        return std::ref(*ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    constexpr auto makeRef(T& value) noexcept {
        return std::ref(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType T>
    constexpr auto makeConstRef(const T& ptr) noexcept {
        return std::cref(std::as_const(*ptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    constexpr auto makeConstRef(const T& value) noexcept {
        return std::cref(value);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    constexpr auto makeObserverStaticArray(Args&&...args) noexcept {
        return makeStaticArray(makeObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    constexpr auto makeConstObserverStaticArray(Args&&...args) noexcept {
        return makeStaticArray(makeConstObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeObserverArray(Args&&...args) noexcept {
        return makeArray(makeObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstObserverArray(Args&&...args) noexcept {
        return makeArray(makeConstObserver(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeRefStaticArray(Args&&...args) noexcept {
        return makeStaticArray(makeRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstRefStaticArray(Args&&...args) noexcept {
        return makeStaticArray(makeConstRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeRefArray(Args&&...args) noexcept {
        return makeArray(makeRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    auto makeConstRefArray(Args&&...args) noexcept {
        return makeArray(makeConstRef(std::forward<Args>(args))...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range Container>
    auto toObserverArray(Container&& container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<value_type *> {};
        vec.reserve(std::size(container));

        std::ranges::transform(container, std::back_inserter(vec), makeObserver);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range Container>
    auto toConstObserverArray(Container&& container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<const value_type *> {};
        vec.reserve(std::size(container));

        std::ranges::transform(container, std::back_inserter(vec), makeConstObserver);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range Container>
    auto toRefArray(Container&& container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<std::reference_wrapper<value_type>> {};
        vec.reserve(std::size(container));

        std::ranges::transform(container, std::back_inserter(vec), std::ref);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range Container>
    auto toConstRefArray(Container&& container) {
        using value_type = typename std::remove_cvref_t<Container>::value_type;

        auto vec = std::vector<const value_type&> {};
        vec.reserve(std::size(container));

        std::ranges::transform(container, std::back_inserter(vec), std::cref);

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range Container>
    auto toArray(Container&& container) {
        auto output = std::vector<typename std::remove_cvref_t<Container>::value_type> {};
        output.reserve(std::size(container));

        std::ranges::copy(container, std::back_inserter(output));

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    requires((core::Is<T, Args> && ...) ||
             (std::convertible_to<T, Args> &&
              ...)) constexpr auto makeStaticArray(T&& first, Args&&...args) noexcept {
        return std::array { std::forward<T>(first), static_cast<T>(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    requires((core::Is<T, Args> && ...) ||
             (std::convertible_to<T, Args> && ...)) auto makeArray(T&& first, Args&&...args) {
        return std::vector { std::forward<T>(first), static_cast<T>(std::forward<Args>(args))... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<SmartPointerType T>
    auto addressOf(const T& ptr) noexcept -> std::intptr_t {
        return addressOf(ptr.get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<RawPointerType T>
    constexpr auto addressOf(const T& ptr) noexcept -> std::intptr_t {
        return reinterpret_cast<std::intptr_t>(ptr.get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::input_range Range,
             UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate>
    constexpr auto copyIf(Range&& input, Predicate&& predicate) noexcept {
        auto output = std::vector<typename std::remove_cvref_t<Range>::value_type> {};
        output.reserve(std::size(input));

        std::ranges::copy_if(input, std::back_inserter(output), std::forward<Predicate>(predicate));

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::input_range Range,
             std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda>
    constexpr auto transform(Range&& input, Lambda&& lambda) noexcept {
        auto output = std::vector<
            std::invoke_result_t<std::remove_cvref_t<Lambda>,
                                 const typename std::remove_cvref_t<Range>::value_type&>> {};
        output.reserve(std::size(input));

        std::ranges::transform(input, std::back_inserter(output), lambda);

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::input_range Range,
             UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate,
             std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda>
    constexpr auto transformIf(Range&& input, Predicate&& predicate, Lambda&& lambda) noexcept {
        auto output = std::vector<
            std::invoke_result_t<std::remove_cvref_t<Lambda>,
                                 const typename std::remove_cvref_t<Range>::value_type&>> {};
        output.reserve(std::size(input));

        std::ranges::for_each(input, [&](auto& elem) {
            if (predicate(elem)) output.emplace_back(lambda(elem));
        });

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<
        std::ranges::input_range Range,
        UnaryPredicateType<typename std::remove_cvref_t<Range>::value_type> Predicate,
        std::invocable<const typename std::remove_cvref_t<Range>::value_type&> Lambda,
        std::output_iterator<
            std::invoke_result_t<Lambda, const typename std::remove_cvref_t<Range>::value_type&>>
            Iterator>
    constexpr auto
        transformIf(Range&& input, Iterator&& it, Predicate&& predicate, Lambda&& lambda) noexcept
        -> void {
        std::ranges::for_each(input, [&](auto& elem) {
            if (predicate(elem)) *it++ = lambda(elem);
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, BinaryPredicateType<std::remove_cvref_t<T>> BinaryPredicate>
    constexpr auto binaryToUnary(T&& value, BinaryPredicate&& predicate) noexcept {
        if constexpr (std::is_rvalue_reference_v<T>)
            return [value     = std::forward<T>(value),
                    predicate = std::forward<BinaryPredicate>(predicate)](
                       const std::remove_cvref_t<T>& other) { return predicate(value, other); };
        else
            return [&value, predicate = std::forward<BinaryPredicate>(predicate)](
                       const std::remove_cvref_t<T>& other) { return predicate(value, other); };
    }

    template<typename T>
    constexpr auto toLambda(T&& t) {
        return [t = std::forward<T>(t)](const auto& elem) -> decltype(t(elem)) { return t(elem); };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto printStacktrace() noexcept -> void {
#ifndef STORMKIT_HAS_STD_STACKTRACE
        using namespace backward;
        auto st = StackTrace {};
        st.load_here(32);

        auto p = Printer {};
        p.print(st);
#else
        auto st = std::stacktrace::current();

        std::cerr << st << std::endl;
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto setupSignalHandler() noexcept -> void {
#ifndef STORMKIT_HAS_STD_STACKTRACE
        auto sh = backward::SignalHandling {};
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto byteSwap(auto value) noexcept -> decltype(auto) {
        auto *ptr = reinterpret_cast<std::byte *>(&value);

        std::reverse(ptr, ptr + sizeof(decltype(value)));

        return value;
    }
} // namespace stormkit::core
