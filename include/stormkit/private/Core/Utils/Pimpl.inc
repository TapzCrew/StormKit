// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::core {
    template<class T, bool Defer = false>
    class Pimpl {
      public:
        Pimpl() noexcept(not Defer);
        ~Pimpl();

        template<IsNot<T> First, class... Args>
        explicit(sizeof...(Args) == 0) Pimpl(First&& first, Args&&... args);

        Pimpl(Pimpl&&) noexcept;
        auto operator=(Pimpl&&) noexcept -> Pimpl&;

        template<class... Args>
        auto init(Args&&... args) -> void;

        auto operator->() noexcept -> T*;
        auto operator->() const noexcept -> const T*;

        auto operator*() noexcept -> T&;
        auto operator*() const noexcept -> const T&;

        auto get() noexcept -> T&;
        auto get() const noexcept -> const T&;

        operator bool() const noexcept;

      private:
        std::unique_ptr<T> m_data;
    };
} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    Pimpl<T, Defer>::Pimpl() noexcept(not Defer) {
        if constexpr (not Defer) init();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    Pimpl<T, Defer>::~Pimpl() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    template<IsNot<T> First, class... Args>
    Pimpl<T, Defer>::Pimpl(First&& first, Args&&... args) {
        init(std::forward<First>(first), std::forward<Args>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    Pimpl<T, Defer>::Pimpl(Pimpl&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::operator=(Pimpl&&) noexcept -> Pimpl& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    template<class... Args>
    auto Pimpl<T, Defer>::init(Args&&... args) -> void {
        m_data = std::make_unique<T>(std::forward<Args>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::operator->() noexcept -> T* {
        return &get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::operator->() const noexcept -> const T* {
        return &get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::operator*() noexcept -> T& {
        return get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::operator*() const noexcept -> const T& {
        return get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::get() noexcept -> T& {
        expects(operator bool());
        return *m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    auto Pimpl<T, Defer>::get() const noexcept -> const T& {
        expects(operator bool());
        return *m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, bool Defer>
    Pimpl<T, Defer>::operator bool() const noexcept {
        return m_data != nullptr;
    }
} // namespace stormkit::core
