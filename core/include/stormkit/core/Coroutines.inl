// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace stormkit::core {
#if !__has_include(<generator>) && !(__has_include(<experimental/generator>) && !defined(__clang__))
    namespace details {
        template<typename T>
        class GeneratorPromise {
          public:
            using value_type     = std::remove_reference_t<T>;
            using reference_type = std::conditional_t<std::is_reference_v<T>, T, T &>;
            using pointer_type   = std::conditional_t<std::is_pointer_v<T>, T, T *>;

            auto get_return_object() noexcept -> Generator<T>;

            constexpr auto initial_suspend() const noexcept -> std::suspend_always;
            constexpr auto final_suspend() const noexcept -> std::suspend_always;

            template<typename U = T, std::enable_if_t<!std::is_rvalue_reference_v<U>, int> = 0>
            auto yield_value(std::remove_reference_t<T> &value) noexcept -> std::suspend_always;

            auto yield_value(std::remove_reference_t<T> &&value) noexcept -> std::suspend_always;

            auto unhandled_exception() noexcept -> void;

            auto return_void() const noexcept -> void;
            auto value() const noexcept -> reference_type;

            template<typename U>
            std::suspend_never await_transform(U &&value) = delete;

            auto rethrow_if_exception() -> void;

          private:
            pointer_type m_value;
            std::exception_ptr m_exception;
        };

        struct GeneratorSentinel {};

        template<typename T>
        class GeneratorIterator {
          public:
            using iterator_category = std::input_iterator_tag;

            using difference_type = std::ptrdiff_t;
            using value_type      = typename GeneratorPromise<T>::value_type;
            using reference       = typename GeneratorPromise<T>::reference_type;
            using pointer         = typename GeneratorPromise<T>::pointer_type;

            using CoroutineHandle = std::coroutine_handle<GeneratorPromise<T>>;

            GeneratorIterator() noexcept;
            explicit GeneratorIterator(CoroutineHandle coroutine) noexcept;
            ~GeneratorIterator() noexcept;

            GeneratorIterator(const GeneratorIterator &) noexcept;
            auto operator=(const GeneratorIterator &) noexcept -> GeneratorIterator &;

            GeneratorIterator(GeneratorIterator &&) noexcept;
            auto operator=(GeneratorIterator &&) noexcept -> GeneratorIterator &;

            friend auto operator==(const GeneratorIterator &, GeneratorSentinel) noexcept -> bool;
            friend auto operator!=(const GeneratorIterator &, GeneratorSentinel) noexcept -> bool;
            friend auto operator==(GeneratorSentinel, const GeneratorIterator &) noexcept -> bool;
            friend auto operator!=(GeneratorSentinel, const GeneratorIterator &) noexcept -> bool;

            auto operator++() -> GeneratorIterator &;

            auto operator++(int) -> void;

            auto next() -> void;

            auto operator*() const noexcept -> reference;
            auto operator->() const noexcept -> pointer;

          private:
            CoroutineHandle m_coroutine = nullptr;
        };

        /////////////////////////////////////
        /////////////////////////////////////
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
#endif

    namespace details {
#define FOR(a, b) for (auto a = T { 0 }; a < b; ++a)
        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a) -> Generator<std::array<T, 1>> {
            FOR(i, a)
            co_yield std::array { i };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b) -> Generator<std::array<T, 2>> {
            FOR(i, a)
            FOR(j, b)
            co_yield std::array { i, j };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b, T c) -> Generator<std::array<T, 3>> {
            FOR(i, a)
            FOR(j, b)
            FOR(k, c)
            co_yield std::array { i, j, k };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b, T c, T d) -> Generator<std::array<T, 4>> {
            FOR(i, a)
            FOR(j, b)
            FOR(k, c)
            FOR(l, d)
            co_yield std::array { i, j, k, l };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b, T c, T d, T e) -> Generator<std::array<T, 5>> {
            FOR(i, a)
            FOR(j, b)
            FOR(k, c)
            FOR(l, d)
            FOR(m, e)
            co_yield std::array { i, j, k, l, m };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b, T c, T d, T e, T f) -> Generator<std::array<T, 6>> {
            FOR(i, a)
            FOR(j, b)
            FOR(k, c)
            FOR(l, d)
            FOR(m, e)
            FOR(n, f)
            co_yield std::array { i, j, k, l, m, n };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<std::integral T>
        auto generateIndices(T a, T b, T c, T d, T e, T f, T g) -> Generator<std::array<T, 7>> {
            FOR(i, a)
            FOR(j, b)
            FOR(k, c)
            FOR(l, d)
            FOR(m, e)
            FOR(n, f)
            FOR(o, g)
            co_yield std::array { i, j, k, l, m, n, o };
        }

#undef FOR

    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::convertible_to<std::size_t>... Args>
    auto generateIndices(Args... args) -> Generator<std::array<std::size_t, sizeof...(Args)>> {
        return generateIndicesAs<std::size_t>(core::as<std::size_t>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range... Args>
    auto generateIndices(const Args &...args)
        -> Generator<std::array<std::size_t, sizeof...(Args)>> {
        return generateIndicesAs<std::size_t>(std::size(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T, std::convertible_to<T>... Args>
    auto generateIndicesAs(Args... args) -> Generator<std::array<T, sizeof...(Args)>> {
        return details::generateIndices<T>(core::as<T>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T, std::ranges::range... Args>
    auto generateIndicesAs(const Args &...args)
        -> Generator<std::array<std::size_t, sizeof...(Args)>> {
        return details::generateIndices<T>(core::as<T>(std::size(args))...);
    }
} // namespace stormkit::core
