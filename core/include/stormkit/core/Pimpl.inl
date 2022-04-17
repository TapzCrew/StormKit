#pragma once

namespace stormkit::core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    Pimpl<T, initialize>::Pimpl() noexcept(!initialize) {
        if constexpr (initialize) m_ptr = std::make_unique<T>();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    template<class... Args>
    Pimpl<T, initialize>::Pimpl(Args &&...args) noexcept(
        !initialize || std::is_nothrow_constructible_v<T, Args...>) {
        if constexpr (initialize) m_ptr = std::make_unique<T>(std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    template<class... Args>
    auto Pimpl<T, initialize>::init(Args &&...args) -> void {
        m_ptr = std::make_unique<T>(std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::operator->() noexcept -> T * {
        STORMKIT_EXPECTS(operator bool());
        return get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::operator->() const noexcept -> const T * {
        STORMKIT_EXPECTS(operator bool());
        return get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::operator*() noexcept -> T & {
        STORMKIT_EXPECTS(operator bool());
        return *m_ptr;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::operator*() const noexcept -> const T & {
        STORMKIT_EXPECTS(operator bool());
        return std::as_const(*m_ptr);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::get() noexcept -> T * {
        return m_ptr.get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    auto Pimpl<T, initialize>::get() const noexcept -> const T * {
        return m_ptr.get();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, bool initialize>
    Pimpl<T, initialize>::operator bool() const noexcept {
        return get() != nullptr;
    }
} // namespace stormkit::core
