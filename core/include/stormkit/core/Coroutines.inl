// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Coroutines.mpp"

/////////////////////////////////////
/////////////////////////////////////
namespace stormkit::core {
    namespace details {
        template<typename T>
        auto GeneratorPromise<T>::get_return_object() noexcept -> Generator<T> {
            using CoroutineHandle = std::coroutine_handle<GeneratorPromise<T>>;

            return Generator<T> { CoroutineHandle::from_promise(*this) };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        constexpr auto GeneratorPromise<T>::initial_suspend() const noexcept
            -> std::suspend_always {
            return {};
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        constexpr auto GeneratorPromise<T>::final_suspend() const noexcept -> std::suspend_always {
            return {};
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        template<typename U, std::enable_if_t<!std::is_rvalue_reference_v<U>, int>>
        auto GeneratorPromise<T>::yield_value(std::remove_reference_t<T> &value) noexcept
            -> std::suspend_always {
            m_value = std::addressof(value);

            return {};
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorPromise<T>::yield_value(std::remove_reference_t<T> &&value) noexcept
            -> std::suspend_always {
            m_value = std::addressof(value);

            return {};
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorPromise<T>::return_void() const noexcept -> void {}

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorPromise<T>::value() const noexcept -> reference_type {
            return static_cast<reference_type>(*m_value);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorPromise<T>::unhandled_exception() noexcept -> void {
            m_exception = std::current_exception();
        }
        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorPromise<T>::rethrow_if_exception() -> void {
            if (m_exception) std::rethrow_exception(m_exception);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        GeneratorIterator<T>::GeneratorIterator() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        GeneratorIterator<T>::GeneratorIterator(CoroutineHandle coroutine) noexcept
            : m_coroutine { coroutine } {}

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        GeneratorIterator<T>::~GeneratorIterator() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        GeneratorIterator<T>::GeneratorIterator(const GeneratorIterator &) noexcept = default;

        template<typename T>
        auto GeneratorIterator<T>::GeneratorIterator::operator=(const GeneratorIterator &) noexcept
            -> GeneratorIterator                            & = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        GeneratorIterator<T>::GeneratorIterator(GeneratorIterator &&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::GeneratorIterator::operator=(GeneratorIterator &&) noexcept
            -> GeneratorIterator                            & = default;

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto operator==(const GeneratorIterator<T> &it, GeneratorSentinel) noexcept -> bool {
            return !it.m_coroutine || it.m_coroutine.done();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto operator!=(const GeneratorIterator<T> &it, GeneratorSentinel s) noexcept -> bool {
            return !(it == s);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto operator==(GeneratorSentinel s, const GeneratorIterator<T> &it) noexcept -> bool {
            return it == s;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto operator!=(GeneratorSentinel s, const GeneratorIterator<T> &it) noexcept -> bool {
            return it != s;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::operator++() -> GeneratorIterator & {
            m_coroutine.resume();

            if (m_coroutine.done()) [[unlikely]]
                m_coroutine.promise().rethrow_if_exception();

            return *this;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::operator++(int) -> void {
            operator++();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::next() -> void {
            operator++();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::operator*() const noexcept -> reference {
            return m_coroutine.promise().value();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
        auto GeneratorIterator<T>::operator->() const noexcept -> pointer {
            return std::addressof(operator*());
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    Generator<T>::Generator(CoroutineHandle handle) noexcept : m_coroutine { handle } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    Generator<T>::~Generator() noexcept {
        if (m_coroutine) [[likely]]
            m_coroutine.destroy();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    Generator<T>::Generator(Generator &&other) noexcept
        : m_coroutine { std::exchange(other.m_coroutine, nullptr) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto Generator<T>::operator=(Generator &&other) noexcept -> Generator & {
        if (&other == this) [[unlikely]]
            return *this;

        m_coroutine = std::exchange(other.m_coroutine, nullptr);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto Generator<T>::begin() noexcept -> iterator {
        if (m_coroutine) [[likely]] {
            m_coroutine.resume();

            if (m_coroutine.done()) [[unlikely]]
                m_coroutine.promise().rethrow_if_exception();
        }

        return iterator { m_coroutine };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto Generator<T>::end() const noexcept -> details::GeneratorSentinel {
        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Func, typename T>
    auto fmap(Func &&func, Generator<T> source) noexcept(noexcept(func()))
        -> Generator<std::invoke_result_t<Func &, typename Generator<T>::iterator::reference>> {
        for (auto &&value : source)
            co_yield std::invoke(std::forward<Func>(func), std::forward<decltype(value)>(value));
    }
} // namespace stormkit::core
