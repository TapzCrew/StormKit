#pragma once

#include <type_traits>

template<typename E>
struct enable_bitmask_operators {
    constexpr enable_bitmask_operators() = default;
    static const bool enable             = false;
};

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
    operator|(E lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
    operator&(E lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs));
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
    operator^(E lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs));
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
    operator~(E lhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    return static_cast<E>(~static_cast<underlying_type>(lhs));
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
    operator|=(E &lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
    return lhs;
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
    operator&=(E &lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs));
    return lhs;
}

template<typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E &>::type
    operator^=(E &lhs, E rhs) noexcept {
    using underlying_type = std::underlying_type_t<E>;
    lhs = static_cast<E>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs));
    return lhs;
}
