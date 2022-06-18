#pragma once

namespace stormkit::core {
    namespace details {
        static std::default_random_engine generator {};
    }

    namespace literals {
        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_kb(core::UInt64 x) noexcept -> core::UInt64 {
            return 1000ULL * x;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_mb(core::UInt64 x) noexcept -> core::UInt64 {
            return 1000ULL * 1000ULL * x;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_gb(core::UInt64 x) noexcept -> core::UInt64 {
            return 1000ULL * 1000ULL * 1000ULL * x;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_kib(core::UInt64 x) noexcept -> core::UInt64 {
            return 1024ULL * x;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_mib(core::UInt64 x) noexcept -> core::UInt64 {
            return 1024ULL * 1024ULL * x;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator""_gib(core::UInt64 x) noexcept -> core::UInt64 {
            return 1024ULL * 1000ULL * 1024ULL * x;
        }
    } // namespace literals

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto seed(UInt32 seed) noexcept -> void { details::generator.seed(seed); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::floating_point T>
    auto rand(T min, T max) noexcept -> T {
        std::uniform_real_distribution<T> dis(min, max);
        return dis(details::generator);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T>
    auto rand(T min, T max) noexcept -> T {
        std::uniform_int_distribution<T> dis(min, max);
        return dis(details::generator);
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
    constexpr auto map(V x, V in_min, V in_max, T out_min, T out_max) noexcept -> T {
        return as<T>((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral auto start, std::integral auto stop>
    constexpr auto range() noexcept -> std::array<decltype(stop - start), stop - start> {
        auto output = std::array<decltype(stop - start), stop - start> {};

        std::iota(std::begin(output), std::end(output), start);

        return output;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T, std::convertible_to<T> U>
    constexpr auto range(T stop, U increment) noexcept -> std::vector<T> {
        return range(T { 0 }, stop, increment);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T, std::convertible_to<T> U>
    constexpr auto range(T start, T stop, U increment) noexcept -> std::vector<T> {
        auto output = std::vector<T> {};
        output.resize(stop - start);

        auto i = 0;
        for (auto &v : output) {
            v = i * core::as<T>(increment);
            ++i;
        }

        return output;
    }
} // namespace stormkit::core
