// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Types.mpp"

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, std::integral _ID>
    template<std::derived_from<T> U>
    constexpr Handle<T, _ID>::operator Handle<U, _ID>() const noexcept {
        return Handle<U, _ID> { id };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, std::integral _ID>
    constexpr auto &Handle<T, _ID>::operator++() noexcept {
        ++id;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, std::integral _ID>
    constexpr auto Handle<T, _ID>::operator++(int) noexcept {
        auto old = *this;
        operator++();
        return old;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, std::integral _ID>
    constexpr auto &Handle<T, _ID>::operator--() noexcept {
        --id;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, std::integral _ID>
    constexpr auto Handle<T, _ID>::operator--(int) noexcept {
        auto old = *this;
        operator--();
        return old;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ByteType... T>
    constexpr auto makeStaticByteArray(T... bytes) noexcept {
        return std::array { bytes... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonByteType... T>
    constexpr auto makeStaticByteArray(T... bytes) noexcept {
        return std::array { static_cast<Byte>(bytes)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range T>
    constexpr auto toByteSpan(T &container) -> core::ByteSpan {
        return core::ByteSpan { reinterpret_cast<Byte *>(std::data(container)),
                                container.size() * sizeof(typename T::value_type) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::ranges::range T>
    constexpr auto toConstByteSpan(const T &container) -> core::ByteConstSpan {
        return core::ByteConstSpan { reinterpret_cast<const Byte *>(std::data(container)),
                                     container.size() * sizeof(typename T::value_type) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType Ptr>
    constexpr auto toByteSpan(Ptr ptr, USize size) -> core::ByteSpan {
        using value_type = std::remove_pointer_t<Ptr>;

        constexpr auto byte_count = []() {
            if constexpr (std::is_same_v<void, std::remove_cvref_t<value_type>>) return 1;
            else
                return sizeof(value_type);
        }();

        return core::ByteSpan { reinterpret_cast<Byte *>(ptr), size * byte_count };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<PointerType Ptr>
    constexpr auto toConstByteSpan(const Ptr ptr, USize size) -> core::ByteConstSpan {
        using value_type = std::remove_pointer_t<Ptr>;

        constexpr auto byte_count = []() {
            if constexpr (std::is_same_v<void, std::remove_cvref_t<value_type>>) return 1;
            else
                return sizeof(value_type);
        }();

        return core::ByteConstSpan { reinterpret_cast<const Byte *>(ptr), size * byte_count };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    requires(!std::ranges::range<T>) constexpr auto toByteSpan(T &value) -> core::ByteSpan {
        return toByteSpan(&value, 1);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<NonPointerType T>
    requires(!std::ranges::range<T>) constexpr auto toConstByteSpan(const T &value)
        -> core::ByteConstSpan {
        return toConstByteSpan(&value, 1);
    }

    namespace literals {
        constexpr auto operator"" _b(unsigned long long int value) noexcept -> Byte {
            return static_cast<Byte>(value);
        }
    } // namespace literals

} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<typename T, std::integral _ID>
constexpr stormkit::core::Hash64 std::hash<stormkit::core::Handle<T, _ID>>::operator()(
    const stormkit::core::Handle<T, _ID> &handle) const noexcept {
    return stormkit::core::as<stormkit::core::Hash64>(handle.id);
}
