#pragma once

namespace storm::core {
    namespace details {
        static std::default_random_engine generator {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto seed(core::UInt32 seed) noexcept -> void { details::generator.seed(seed); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto rand(T &&min, T &&max) noexcept -> T {
        using Type = std::decay_t<std::remove_cv_t<T>>;
        if constexpr (std::is_floating_point_v<Type>) {
            std::uniform_real_distribution<Type> dis(min, max);
            return dis(details::generator);
        } else {
            std::uniform_int_distribution<Type> dis(min, max);
            return dis(details::generator);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename V, typename>
    constexpr auto realIsEqual(T &&a, V &&b) noexcept {
        return std::abs(a - b) < std::numeric_limits<std::remove_cv_t<std::decay_t<T>>>::epsilon();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename V>
    constexpr auto map(V x, V in_min, V in_max, T out_min, T out_max) noexcept -> T {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T>
    auto ptrToAddress(const T *ptr) noexcept -> std::intptr_t {
        return reinterpret_cast<std::intptr_t>(ptr);
    }
} // namespace storm::core
