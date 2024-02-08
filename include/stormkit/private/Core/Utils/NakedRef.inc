// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
export namespace stormkit::core {
    template<typename T>
    class NakedRef {
      public:
        using UnderlyingType  = T;
        using underlying_type = UnderlyingType;

        using PointerType  = UnderlyingType*;
        using pointer_type = PointerType;

        using ReferenceType  = UnderlyingType&;
        using reference_type = ReferenceType;

        constexpr NakedRef(ReferenceType value) noexcept;
        constexpr NakedRef(PointerType value) noexcept;
        constexpr ~NakedRef() noexcept;

        constexpr NakedRef(const NakedRef&) noexcept;
        constexpr NakedRef(NakedRef&&) noexcept;

        constexpr auto operator=(const NakedRef&) noexcept -> NakedRef&;
        constexpr auto operator=(NakedRef&&) noexcept -> NakedRef&;

        [[nodiscard]] constexpr auto get(this auto&& self) noexcept -> decltype(auto);
        [[nodiscard]] constexpr auto operator->(this auto&& self) noexcept -> decltype(auto);
        [[nodiscard]] constexpr auto operator*(this auto&& self) noexcept -> decltype(auto);

        [[nodiscard]] constexpr operator T&(this auto&& self) noexcept;

      private:
        PointerType m_value;
    };

    template<typename T>
    using OptionalRef = std::optional<NakedRef<T>>;

    template<typename T>
        requires IsRawPointer<T> or IsLValueReference<T>
    constexpr auto makeNakedRef(T&& value) noexcept -> decltype(auto);

    constexpr auto makeNakedRef(IsFancyPointer auto& value) noexcept -> decltype(auto);

    template<typename... Args>
        requires((IsLValueReference<Args> or IsPointer<Args>) && ...)
    constexpr auto makeNakedRefArray(Args&&... args);

    template<typename... Args>
        requires((IsLValueReference<Args> or IsPointer<Args>) && ...)
    constexpr auto makeNakedRefVector(Args&&... args);
} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::NakedRef(ReferenceType value) noexcept : m_value { &value } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::NakedRef(PointerType value) noexcept : m_value { value } {
        core::expects(m_value != nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::~NakedRef() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::NakedRef(const NakedRef&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::NakedRef(NakedRef&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto NakedRef<T>::operator=(const NakedRef&) noexcept -> NakedRef& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto NakedRef<T>::operator=(NakedRef&&) noexcept -> NakedRef& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    [[nodiscard]] constexpr auto NakedRef<T>::get(this auto&& self) noexcept -> decltype(auto) {
        return *std::forward<decltype(self)>(self).m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto NakedRef<T>::operator->(this auto&& self) noexcept -> decltype(auto) {
        return std::forward<decltype(self)>(self).m_value;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto NakedRef<T>::operator*(this auto&& self) noexcept -> decltype(auto) {
        return std::forward<decltype(self)>(self).get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr NakedRef<T>::operator T&(this auto&& self) noexcept {
        return std::forward<decltype(self)>(self).get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires IsRawPointer<T> or IsLValueReference<T>
    constexpr auto makeNakedRef(T&& value) noexcept -> decltype(auto) {
        return NakedRef<std::remove_reference_t<std::remove_pointer_t<T>>> { value };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto makeNakedRef(IsFancyPointer auto& value) noexcept -> decltype(auto) {
        return NakedRef<typename PureType<decltype(value)>::value_type> { value.get() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires((IsLValueReference<Args> or IsPointer<Args>) && ...)
    constexpr auto makeNakedRefArray(Args&&... args) {
        return std::array { (makeNakedRef(std::forward<Args>(args)), ...) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
        requires((IsLValueReference<Args> or IsPointer<Args>) && ...)
    constexpr auto makeNakedRefVector(Args&&... args) {
        return std::vector { (makeNakedRef(std::forward<Args>(args)), ...) };
    }
} // namespace stormkit::core
