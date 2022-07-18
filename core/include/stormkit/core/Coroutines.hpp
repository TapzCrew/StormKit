// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <array>
#include <coroutine>
#include <exception>
#include <utility>

#if __has_include(<generator>)
    #include <generator>
#elif __has_include(<experimental/generator>) && !defined(__clang__)
    #include <experimental/generator>
#else
    #include <iterator>
#endif

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Concepts.hpp>

namespace stormkit::core {
#if __has_include(<generator>)
    template<typename T>
    using Generator = std::generator<T>;
#elif __has_include(<experimental/generator>) && !defined(__clang__)
    template<typename T>
    using Generator = std::experimental::generator<T>;
#else

    template<typename T>
    class Generator;

    namespace details {
        template<typename T>
        class GeneratorPromise;

        template<typename T>
        class GeneratorIterator;

        struct GeneratorSentinel;
    } // namespace details

    template<typename T>
    class Generator {
      public:
        using promise_type = details::GeneratorPromise<T>;
        using iterator     = details::GeneratorIterator<T>;

        ~Generator() noexcept;

        Generator(const Generator &) = delete;
        auto operator=(const Generator &) -> Generator & = delete;

        Generator(Generator &&) noexcept;
        auto operator=(Generator &&) noexcept -> Generator &;

        auto begin() noexcept -> iterator;
        auto end() const noexcept -> details::GeneratorSentinel;

      private:
        using CoroutineHandle = std::coroutine_handle<details::GeneratorPromise<T>>;

        friend class details::GeneratorPromise<T>;

        explicit Generator(CoroutineHandle handle) noexcept;

        CoroutineHandle m_coroutine;
    };

#endif

    template<typename Func, typename T>
    auto fmap(Func &&func, Generator<T> source) noexcept(noexcept(func()))
        -> Generator<std::invoke_result_t<Func &, typename Generator<T>::iterator::reference>>;

    template<std::convertible_to<std::size_t>... Args>
    auto generateIndices(Args... args) -> Generator<std::array<std::size_t, sizeof...(Args)>>;

    template<std::ranges::range... Args>
    auto generateIndices(const Args &...args)
        -> Generator<std::array<std::size_t, sizeof...(Args)>>;
} // namespace stormkit::core

#include "Coroutines.inl"
