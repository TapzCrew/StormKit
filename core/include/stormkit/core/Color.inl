// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Color.mpp"

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    constexpr inline auto convertColorComponent(UInt8 component) noexcept -> float {
        return as<float>(component) / 255.f;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr inline auto convertColorComponent(float component) noexcept -> UInt8 {
        return as<UInt8>(component * 255u);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr auto maxColorComponentValue() noexcept -> T {
        if constexpr (std::is_same_v<T, float>) return 1.f;

        return 255u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr RGBColor<T>::RGBColor(T _red, T _green, T _blue, T _alpha) noexcept
        : red { _red }, green { _green }, blue { _blue }, alpha { _alpha } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr RGBColor<T>::RGBColor(const Vector3<T> &vector) noexcept
        : red { vector.r }, green { vector.g }, blue { vector.b },
          alpha(maxColorComponentValue<T>()) {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr RGBColor<T>::RGBColor(const Vector4<T> &vector) noexcept
        : red { vector.r }, green { vector.g }, blue { vector.b }, alpha { vector.a } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    template<ColorComponentType U>
    constexpr RGBColor<T>::RGBColor(const RGBColor<U> &other) noexcept
        : red { convertColorComponent(other.red) }, green { convertColorComponent(other.green) },
          blue { convertColorComponent(other.blue) }, alpha { convertColorComponent(other.alpha) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    template<ColorComponentType U>
    constexpr auto RGBColor<T>::operator=(const RGBColor<U> &other) noexcept -> RGBColor<T> & {
        if (this == &other) return *this;

        red   = convertColorComponent(other.red);
        green = convertColorComponent(other.green);
        blue  = convertColorComponent(other.blue);
        alpha = convertColorComponent(other.alpha);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr auto RGBColor<T>::toVector3() const noexcept -> Vector3<T> {
        return { red, green, blue };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr auto RGBColor<T>::toVector4() const noexcept -> Vector4<T> {
        return { red, green, blue, alpha };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr RGBColor<T>::operator Vector3<T>() const noexcept {
        return toVector3();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ColorComponentType T>
    constexpr RGBColor<T>::operator Vector4<T>() const noexcept {
        return toVector4();
    }
} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::ColorComponentType T>
constexpr auto std::hash<stormkit::core::RGBColor<T>>::operator()(
    const stormkit::core::RGBColor<T> &color) const noexcept -> stormkit::core::Hash64 {
    auto hash = stormkit::core::Hash64 {};
    stormkit::core::hashCombine(hash, color.red);
    stormkit::core::hashCombine(hash, color.green);
    stormkit::core::hashCombine(hash, color.blue);
    stormkit::core::hashCombine(hash, color.alpha);

    return hash;
}

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::ColorComponentType T>
constexpr auto std::equal_to<stormkit::core::RGBColor<T>>::operator()(
    const stormkit::core::RGBColor<T> &first,
    const stormkit::core::RGBColor<T> &second) const noexcept -> bool {
    const auto hasher = hash<stormkit::core::RGBColor<T>> {};
    return hasher(first) == hasher(second);
}
