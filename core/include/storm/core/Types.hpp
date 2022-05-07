// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <storm/core/Format.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <vector>

#include <compare>

/////////// - StormKit::core - ///////////
#include <storm/core/Concepts.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Format.hpp>

namespace storm::core {
    /// \group uint-types Unsigned types
    /// Unsigned integer type with width of exactly 8, 16, 32 and 64 bits respectively.
    /// \unique_name UInt8
    using UInt8 = std::uint8_t;

    /// \group uint-types
    /// \unique_name UInt16
    using UInt16 = std::uint16_t;

    /// \group uint-types
    /// \unique_name UInt32
    using UInt32 = std::uint32_t;

    /// \group uint-types
    /// \unique_name UInt64
    using UInt64 = std::uint64_t;

    /// \group int-types Signed types
    /// Signed integer type with width of exactly 8, 16, 32 and 64 bits respectively.
    /// \unique_name Int8
    using Int8 = std::int8_t;

    /// \group int-types
    /// \unique_name Int16
    using Int16 = std::int16_t;

    /// \group int-types
    /// \unique_name Int32
    using Int32 = std::int32_t;

    /// \group int-types
    /// \unique_name Int64
    using Int64 = std::int64_t;

    /// \brief `ArraySize` is a type that can hold the maximum size of an array.
    /// \unique_name ArraySize
    using ArraySize = std::size_t;

    /// \brief `Hash32` is a type that old a 32 bit hash
    /// \unique_name Hash32
    using Hash32 = UInt32;

    /// \brief `Hash64` is a type that old a 64 bit hash
    /// \unique_name Hash64
    using Hash64 = UInt64;

    /// \brief `Byte` is a type that implements the concept of byte.
    ///
    /// It's not an arithmeric type, a byte is only a collection of bits and the only operators
    /// defined for it are the bitwises ones.
    /// \unique_name Byte
    using Byte = std::byte;

    /// \brief `ByteArray` is a dynamic array that contain
    /// [`bytes`](standardese://*Byte/).
    /// \unique_name ByteArray
    using ByteArray = std::vector<Byte>;

    /// \group byteStaticArray ByteStaticArray
    /// \brief `ByteStaticArray` is a static array of `size` that contain
    /// [`bytes`](standardese://*Byte/).
    ///
    /// The size is defined at compilation.
    /// \unique_name ByteStaticArray
    template<ArraySize size>
    using ByteStaticArray = std::array<Byte, size>;

    /// \brief `ByteArray` is a span of [`bytes`](standardese://*Byte/).
    /// \unique_name ByteSpan
    using ByteSpan = core::span<Byte>;

    /// \brief `ByteArray` is a span of const [`bytes`](standardese://*Byte/).
    /// \unique_name ByteConstSpan
    using ByteConstSpan = core::span<const Byte>;

    using Secondf = std::chrono::duration<float, std::chrono::seconds::period>;

    /// \brief `Handle32` is a type wich represent a 32bit wide ID of an object
    ///
    /// This type is meant to be specialized for a type of object `T`
    /// \unique_name Handle32
    template<typename T>
    struct Handle32 {
        static constexpr auto INVALID_HANDLE = std::numeric_limits<UInt32>::max();

        UInt32 id = INVALID_HANDLE;

        [[nodiscard]] constexpr std::strong_ordering
            operator<=>(const Handle32<T> &) const noexcept = default;

        template<std::derived_from<T> U>
        constexpr operator Handle32<U>() const noexcept {
            return Handle32<U> { id };
        }

        constexpr auto &operator++() noexcept {
            ++id;
            return *this;
        }

        constexpr auto operator++(int) noexcept {
            auto old = *this;
            operator++();
            return old;
        }

        constexpr auto &operator--() noexcept {
            --id;
            return *this;
        }

        constexpr auto operator--(int) noexcept {
            auto old = *this;
            operator--();
            return old;
        }
    };

    /// \brief `Handle64` is a type wich represent a 64bit wide ID of an object
    ///
    /// This type is meant to be specialized for a type of object `T`
    /// \unique_name Handle64
    template<typename T>
    struct Handle64 {
        static constexpr auto INVALID_HANDLE = std::numeric_limits<UInt64>::max();

        UInt64 id = INVALID_HANDLE;

        [[nodiscard]] constexpr std::strong_ordering
            operator<=>(const Handle64<T> &) const noexcept = default;

        template<std::derived_from<T> U>
        constexpr operator Handle64<U>() const noexcept {
            return Handle64<U> { id };
        }

        constexpr auto &operator++() noexcept {
            ++id;
            return *this;
        }

        constexpr auto operator++(int) noexcept {
            auto old = *this;
            operator++();
            return old;
        }

        constexpr auto &operator--() noexcept {
            --id;
            return *this;
        }

        constexpr auto operator--(int) noexcept {
            auto old = *this;
            operator--();
            return old;
        }
    };

    template<BYTE_CONCEPT(... T)>
    constexpr auto makeStaticByteArray(T... bytes) noexcept {
        return std::array { static_cast<storm::core::Byte>(bytes)... };
    }

    template<RANGE_CONCEPT(Container)>
    constexpr auto toByteSpan(Container &container) {
        return core::span<Byte>(reinterpret_cast<Byte *>(std::data(container)),
                                std::size(container) *
                                    sizeof(typename std::remove_cvref_t<Container>::value_type));
    }

    template<RANGE_CONCEPT(Container)>
    constexpr auto toConstByteSpan(const Container &container) {
        return core::span<const Byte>(reinterpret_cast<const Byte *>(std::data(container)),
                                      std::size(container) *
                                          sizeof(
                                              typename std::remove_cvref_t<Container>::value_type));
    }

    template<POINTER_CONCEPT(Ptr)>
    constexpr auto toByteSpan(Ptr ptr, ArraySize size) {
        using element_type = std::remove_pointer_t<Ptr>;

        constexpr auto _size = []() {
            if constexpr (std::is_same_v<void, element_type>) return 1;
            else
                return sizeof(element_type);
        }();

        return core::span<Byte>(reinterpret_cast<Byte *>(ptr), size * _size);
    }

    template<POINTER_CONCEPT(Ptr)>
    constexpr auto toConstByteSpan(const Ptr ptr, ArraySize size) {
        using element_type = std::remove_pointer_t<Ptr>;

        constexpr auto _size = []() {
            if constexpr (std::is_same_v<void, element_type>) return 1;
            else
                return sizeof(element_type);
        }();

        return core::span<const Byte>(reinterpret_cast<const Byte *>(ptr), size * _size);
    }

    template<POINTER_CONCEPT(Ptr)>
    constexpr auto toByteSpan(Ptr ptr) {
        using element_type = std::remove_pointer_t<Ptr>;

        constexpr auto _size = []() {
            if constexpr (std::is_same_v<void, element_type>) return 1;
            else
                return sizeof(element_type);
        }();

        return core::span<Byte>(reinterpret_cast<Byte *>(ptr), _size);
    }

    template<POINTER_CONCEPT(Ptr)>
    constexpr auto toConstByteSpan(const Ptr ptr) {
        using element_type = std::remove_pointer_t<Ptr>;

        constexpr auto _size = []() {
            if constexpr (std::is_same_v<void, element_type>) return 1;
            else
                return sizeof(element_type);
        }();

        return core::span<const Byte>(reinterpret_cast<const Byte *>(ptr), _size);
    }
} // namespace storm::core

CUSTOM_FORMAT(storm::core::Secondf, std::to_string(data.count()));

CUSTOM_FORMAT(storm::core::Byte, std::to_string(static_cast<std::uint16_t>(data)))

namespace std {
    template<typename T>
    struct hash<storm::core::Handle32<T>> {
        [[nodiscard]] constexpr storm::core::Hash64
            operator()(const storm::core::Handle32<T> &handle) const noexcept {
            return static_cast<storm::core::Hash64>(handle.id);
        }
    };

    template<typename T>
    struct hash<storm::core::Handle64<T>> {
        [[nodiscard]] constexpr storm::core::Hash64
            operator()(const storm::core::Handle64<T> &handle) const noexcept {
            return static_cast<storm::core::Hash64>(handle.id);
        }
    };
} // namespace std
