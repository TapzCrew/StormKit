#pragma once

#include "Numerics.mpp"

namespace stormkit::core {
    static std::default_random_engine generator {};

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto seed(UInt32 seed) noexcept -> void { generator.seed(seed); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point T>
    auto rand(T min, T max) noexcept -> T {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(generator);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T>
    auto rand(T min, T max) noexcept -> T {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(generator);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point T>
    constexpr auto isEqual(T a, T b) noexcept -> bool {
        return std::abs((a - b) < std::numeric_limits<T>::epsilon());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point T, std::floating_point V>
    constexpr auto isEqual(T a, V b) noexcept -> bool {
        using type = details::SafeTypeHelperType<T, V>;

        return std::abs(core::as<type>(a) - core::as<type>(b)) <
               std::numeric_limits<type>::epsilon();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T>
    constexpr auto isEqual(T a, T b) noexcept -> bool {
        return a == b;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T, std::integral V>
    constexpr auto isEqual(T a, V b) noexcept -> bool {
        using type = details::SafeTypeHelperType<T, V>;

        return core::as<type>(a) == core::as<type>(b);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T, ArithmeticType V>
    constexpr auto map(V x, V in_min, V in_max, T out_min, T out_max) noexcept {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
} // namespace stormkit::core
