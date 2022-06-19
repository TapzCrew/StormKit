// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr Extent<T>::Extent() noexcept : Extent { 0, 0 } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr Extent<T>::Extent(UnderlyingType _width,
                                UnderlyingType _height,
                                UnderlyingType _depth) noexcept
        : width { _width }, height { _height }, depth { _depth } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    template<ArithmeticType U>
    constexpr Extent<T>::Extent(const Extent<U>& other) noexcept
        : width { as<UnderlyingType>(other.width) }, height { as<UnderlyingType>(other.height) },
          depth { as<UnderlyingType>(other.depth) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator<=>(const Extent<T>& other) const noexcept -> OrderingType {
        if (auto ret = width <=> other.width; ret != 0) return ret;
        if (auto ret = height <=> other.height; ret != 0) return ret;
        if (auto ret = depth <=> other.depth; ret != 0) return ret;

        return OrderingType::equivalent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator==(const Extent<T>& other) const noexcept -> bool {
        return isEqual<T>(width, other.width) && isEqual<T>(height, other.height) &&
               isEqual<T>(depth, other.depth);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator*(UnderlyingType factor) const noexcept -> Extent {
        return Extent { width, height, depth } *= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator/(UnderlyingType factor) const noexcept -> Extent {
        return Extent { width, height, depth } /= factor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator*=(UnderlyingType factor) noexcept -> Extent& {
        width *= factor;
        height *= factor;
        depth *= factor;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<ArithmeticType T>
    constexpr auto Extent<T>::operator/=(UnderlyingType factor) noexcept -> Extent& {
        width /= factor;
        height /= factor;
        depth /= factor;
        return *this;
    }
} // namespace stormkit::core

/////////////////////////////////////
/////////////////////////////////////
template<stormkit::core::ArithmeticType T>
constexpr stormkit::core::Hash64 std::hash<stormkit::core::Extent<T>>::operator()(
    const stormkit::core::Extent<T>& extent) const noexcept {
    auto hash = stormkit::core::Hash64 { 0 };
    stormkit::core::hashCombine(hash, extent.width);
    stormkit::core::hashCombine(hash, extent.height);
    stormkit::core::hashCombine(hash, extent.depth);

    return hash;
}
