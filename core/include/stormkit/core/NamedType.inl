// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr NamedType<T, Tag, Skills...>::NamedType() noexcept(
        std::is_nothrow_default_constructible_v<UnderlyingType>) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr NamedType<T, Tag, Skills...>::~NamedType() noexcept(
        std::is_nothrow_destructible_v<UnderlyingType>) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr NamedType<T, Tag, Skills...>::NamedType(const NamedType &) noexcept(
        std::is_nothrow_copy_constructible_v<UnderlyingType>) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr auto NamedType<T, Tag, Skills...>::operator=(const NamedType &) noexcept(
        std::is_nothrow_copy_assignable_v<UnderlyingType>) -> NamedType & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr NamedType<T, Tag, Skills...>::NamedType(NamedType &&) noexcept(
        std::is_nothrow_move_constructible_v<UnderlyingType>) = default;

    // clang-format off
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr auto NamedType<T, Tag, Skills...>::operator=(NamedType &&) noexcept(
        std::is_nothrow_move_assignable_v<UnderlyingType>) -> NamedType & = default;
    // clang-format on

    /////////////////////////////////////
    /////////////////////////////////////
    /*template<class T, class Tag, template<class> class... Skills>
    template<IsNot<NamedType<T, Tag, Skills...>>... Args>
    constexpr NamedType<T, Tag, Skills...>::NamedType(Args &&...args) noexcept(
        std::is_nothrow_constructible_v<UnderlyingType, Args...>)
        : m_value { std::forward<Args>(args)... } {} */

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr auto NamedType<T, Tag, Skills...>::get() noexcept
        -> std::remove_reference_t<UnderlyingType> & {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr auto NamedType<T, Tag, Skills...>::get() const noexcept
        -> const std::remove_reference_t<UnderlyingType> & {
        return m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Tag, template<class> class... Skills>
    constexpr NamedType<T, Tag, Skills...>::operator Ref() {
        return Ref { m_value };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class... Args>
    constexpr auto makeNamed(Args &&...args) noexcept -> T {
        return T { std::forward<Args>(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto PreIncrementable<T>::operator++() -> T & {
        ++this->underlying().get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto PostIncrementable<T>::operator++(int) -> T {
        return T { this->underlying().get()++ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto PreDecrementable<T>::operator--() -> T {
        --this->underlying().get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto PostDecrementable<T>::operator--(int) -> T {
        return T { this->underlying().get()-- };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BinaryAddable<T>::operator+(T const &other) const -> T {
        return T { this->underlying().get() + other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BinaryAddable<T>::operator+=(T const &other) -> T & {
        this->underlying().get() += other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto UnaryAddable<T>::operator+() const -> T {
        return T { +this->underlying().get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BinarySubtractable<T>::operator-(T const &other) const -> T {
        return T { this->underlying().get() - other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BinarySubtractable<T>::operator-=(T const &other) -> T & {
        this->underlying().get() -= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto UnarySubtractable<T>::operator-() const -> T {
        return T { -this->underlying().get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Multiplicable<T>::operator*(T const &other) const -> T {
        return T { this->underlying().get() * other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Multiplicable<T>::operator*=(T const &other) -> T & {
        this->underlying().get() *= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Divisible<T>::operator/(T const &other) const -> T {
        return T { this->underlying().get() / other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Divisible<T>::operator/=(T const &other) -> T & {
        this->underlying().get() /= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Modulable<T>::operator%(T const &other) const -> T {
        return T { this->underlying().get() % other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Modulable<T>::operator%=(T const &other) -> T & {
        this->underlying().get() %= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseInvertable<T>::operator~() const -> T {
        return T { ~this->underlying().get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseAndable<T>::operator&(T const &other) -> T const {
        return T { this->underlying().get() & other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseAndable<T>::operator&=(T const &other) -> T & {
        this->underlying().get() &= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseOrable<T>::operator|(T const &other) const -> T {
        return T { this->underlying().get() | other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseOrable<T>::operator|=(T const &other) -> T & {
        this->underlying().get() |= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseXorable<T>::operator^(T const &other) const -> T {
        return T { this->underlying().get() ^ other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseXorable<T>::operator^=(T const &other) -> T & {
        this->underlying().get() ^= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseLeftShiftable<T>::operator<<(T const &other) const -> T {
        return T { this->underlying().get() << other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseLeftShiftable<T>::operator<<=(T const &other) -> T & {
        this->underlying().get() <<= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseRightShiftable<T>::operator>>(T const &other) const -> T {
        return T { this->underlying().get() >> other.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto BitWiseRightShiftable<T>::operator>>=(T const &other) -> T & {
        this->underlying().get() >>= other.get();
        return this->underlying();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    constexpr auto Comparable<T>::operator<=>(const T &other) const noexcept
        -> std::strong_ordering {
        return this->underlying().get() <=> other.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr auto Dereferencable<NamedType<T, Parameter, Skills...>>::operator*()
        & -> std::remove_reference_t<T> & {
        return this->underlying().get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr auto Dereferencable<NamedType<T, Parameter, Skills...>>::operator*()
        const & -> const std::remove_reference_t<T> & {
        return this->underlying().get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    /*template<class Destination>
    template<class T>
    constexpr ImplicitlyConvertibleTo<Destination>::template templ<T>::operator Destination()
        const {
        return this->underlying().get();
    }*/ // TODO make it build

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr FunctionCallable<
        NamedType<T, Parameter, Skills...>>::operator std::remove_reference_t<T> &() {
        return this->underlying().get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr FunctionCallable<
        NamedType<T, Parameter, Skills...>>::operator const std::remove_reference_t<T> &() const {
        return this->underlying().get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr auto MethodCallable<NamedType<T, Parameter, Skills...>>::operator->()
        -> std::remove_reference_t<T> * {
        return std::addressof(this->underlying().get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, class Parameter, template<class> class... Skills>
    constexpr auto MethodCallable<NamedType<T, Parameter, Skills...>>::operator->() const
        -> const std::remove_reference_t<T> * {
        return std::addressof(this->underlying().get());
    }
} // namespace stormkit::core
