// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <functional>
#include <iostream>
#include <memory>

#include <storm/core/Memory.hpp>
#include <storm/core/TypeTraits.hpp>

namespace storm::core {
    class NamedTypeTrait {};

    template<typename T>
    using is_named_type = std::is_base_of<NamedTypeTrait, T>;

    template<typename T>
    inline constexpr bool is_named_type_v = is_named_type<T>::value;

    template<typename T, typename Tag, template<typename> class... Skills>
    class NamedType: public NamedTypeTrait, public Skills<NamedType<T, Tag, Skills...>>... {
      public:
        using UnderlyingType = T;

        template<typename... Args>
        explicit(sizeof...(Args) == 1) constexpr NamedType(Args &&...args) noexcept
            : m_value { std::forward<Args>(args)... } {}

        constexpr NamedType(T &&value) noexcept : m_value { std::forward<T>(value) } {}

        constexpr T &get() noexcept { return m_value; }

        constexpr T const &get() const noexcept { return m_value; }

        constexpr operator T &() noexcept { return get(); }

        constexpr operator const T &() const noexcept { return get(); }

        constexpr operator const T() const noexcept { return m_value; }

        constexpr const T &operator*() const noexcept { return get(); }

        constexpr T &operator*() noexcept { return get(); }

        constexpr const T *operator->() const noexcept { return &get(); }

        constexpr T *operator->() noexcept { return &get(); }

      private:
        T m_value;
    };

    template<class NamedType, typename... Args>
    constexpr NamedType makeNamed(Args &&...args) noexcept {
        return NamedType(typename NamedType::UnderlyingType(std::forward<Args>(args)...));
    }
} // namespace storm::core

namespace std {
    template<typename T, typename Tag, template<typename> class... Skills>
    struct hash<storm::core::NamedType<T, Tag, Skills...>> {
        using NamedType       = storm::core::NamedType<T, Tag, Skills...>;
        using checkIfHashable = typename std::enable_if<NamedType::is_hashable, void>::type;

        size_t operator()(storm::core::NamedType<T, Tag, Skills...> const &x) const noexcept {
            return std::hash<T>()(*x);
        }
    };
} // namespace std
