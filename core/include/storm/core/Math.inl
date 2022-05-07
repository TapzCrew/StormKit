// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Math.hpp"

namespace storm::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr Extent<T>::Extent(T _width, T _height, T _depth) noexcept
        : width { _width }, height { _height }, depth { _depth } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    template<ARITHMETIC_TYPE_CONCEPT(U)>
    constexpr Extent<T>::Extent(const Extent<U> &other) noexcept
        : width { static_cast<T>(other.width) }, height { static_cast<T>(other.height) }, depth {
              static_cast<T>(other.depth)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr bool Extent<T>::operator==(const Extent<T> &other) const noexcept {
        if constexpr (std::is_floating_point_v<T>)
            return realIsEqual(width, other.width) && realIsEqual(height, other.height) &&
                   realIsEqual(depth, other.depth);
        else
            return width == other.width && height == other.height && depth == other.depth;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr Extent<T> Extent<T>::operator*(T factor) const noexcept {
        return Extent<T> { width, height, depth } *= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr Extent<T> Extent<T>::operator/(T factor) const noexcept {
        return Extent<T> { width, height, depth } /= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr Extent<T> &Extent<T>::operator*=(T factor) noexcept {
        width *= factor;
        height *= factor;
        depth *= factor;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    constexpr Extent<T> &Extent<T>::operator/=(T factor) noexcept {
        width /= factor;
        height /= factor;
        depth /= factor;
        return *this;
    }
} // namespace storm::core
