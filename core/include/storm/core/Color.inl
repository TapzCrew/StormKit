// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr T maxColorComponentValue() noexcept {
        if constexpr (std::is_same_v<T, float>) return 1.f;

        return 255u;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr RGBColor<T>::RGBColor(T _red, T _green, T _blue, T _alpha) noexcept
        : red { _red }, green { _green }, blue { _blue }, alpha { _alpha } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr RGBColor<T>::RGBColor(const Vector3<T> &vector) noexcept
        : red { vector.r }, green { vector.g }, blue { vector.b },
          alpha(maxColorComponentValue<T>()) {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr RGBColor<T>::RGBColor(const Vector4<T> &vector) noexcept
        : red { vector.r }, green { vector.g }, blue { vector.b }, alpha { vector.a } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    template<COLOR_COMPONENT_CONCEPT(U)>
    constexpr RGBColor<T>::RGBColor(const RGBColor<U> &other) noexcept
        : red { convertColorComponent(other.red) }, green { convertColorComponent(other.green) },
          blue { convertColorComponent(other.blue) }, alpha { convertColorComponent(other.alpha) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    template<COLOR_COMPONENT_CONCEPT(U)>
    constexpr RGBColor<T> &RGBColor<T>::operator=(const RGBColor<U> &other) noexcept {
        if (this == &other) return *this;

        red   = convertColorComponent(other.red);
        green = convertColorComponent(other.green);
        blue  = convertColorComponent(other.blue);
        alpha = convertColorComponent(other.alpha);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr Vector3<T> RGBColor<T>::toVector3() const noexcept {
        return { red, green, blue };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr Vector4<T> RGBColor<T>::toVector4() const noexcept {
        return { red, green, blue, alpha };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr RGBColor<T>::operator Vector3<T>() const noexcept {
        return toVector3();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr RGBColor<T>::operator Vector4<T>() const noexcept {
        return toVector4();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr inline float convertColorComponent(UInt8 component) noexcept {
        return static_cast<float>(component) / 255.f;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr inline UInt8 convertColorComponent(float component) noexcept {
        return static_cast<UInt8>(component * 255u);
    }
} // namespace storm::core
