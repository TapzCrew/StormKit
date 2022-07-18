// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <type_traits>

#include <stormkit/core/Fwd.hpp>
#include <stormkit/core/Hash.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    /// \brief Convert a color component from [`UInt8`](standardese://UInt8/) to
    /// `float`.
    ///
    /// The component will be remaped from [0, 255] to [0, 1.f].
    ///
    /// \param component The component to convert.
    /// \returns The converted component.
    /// \unique_name convertColorComponent(UInt8)
    constexpr auto convertColorComponent(UInt8 component) noexcept -> float;

    /// \brief Convert a color component from `float` to
    /// [`UInt8`](standardese://UInt8).
    ///
    /// The component will be remaped from [0, 1.f] to [0, 255].
    ///
    /// \param component The component to convert.
    /// \returns The converted component.
    /// \unique_name convertColorComponent(float)
    constexpr auto convertColorComponent(float component) noexcept -> UInt8;

    /// \brief Get the max value of a component with from a given Type.
    /// \returns The maximum value of `T` color component
    /// \unique_name maxColorComponentValue<T>()
    template<ColorComponentType T>
    constexpr auto maxColorComponentValue() noexcept -> T;

    /// \brief `RGBColor` is a representation of a RGB/RGBA color.
    ///
    /// This class can use as component storage type `float` and
    /// [`UInt8`](standardese://UInt8).
    /// \requires Type `T` shall be either `float` or [`UInt8`](standardese://UInt8).
    /// \unique_name RGBColor<T>
    template<ColorComponentType T>
    struct RGBColor {
        using UnderlyingType = T;
        /// \output_section Public Special Member Functions
        /// \brief Construct a color from red, gree, blue and optionaly alpha.
        /// \param red The amound of red.
        /// \param green The amound of green.
        /// \param blue The amound of blue.
        /// \param alpha The amound of alpha (default constructed to the
        /// `maxColorComponentValue` of the given T type).
        constexpr RGBColor(
            UnderlyingType red,
            UnderlyingType green,
            UnderlyingType blue,
            UnderlyingType alpha = maxColorComponentValue<UnderlyingType>()) noexcept;

        /// \brief Construct a color from a [`Vector3`](standardese://Vector3).
        /// \param vector The input [`Vector3`](standardese://Vector3).
        /// \notes The alpha value is constructed from `maxColorComponentValue`.
        constexpr explicit RGBColor(const Vector3<UnderlyingType> &vector) noexcept;

        /// \brief Construct a color from a [`Vector4`](standardese://Vector4).
        /// \param vector The input [`Vector4`](standardese://Vector4).
        constexpr explicit RGBColor(const Vector4<UnderlyingType> &vector) noexcept;

        /// \brief Copy constructor.
        ///
        /// Construct a copy of other.
        ///
        /// \param other The copied color.
        constexpr RGBColor(const RGBColor<UnderlyingType> &other) noexcept = default;

        /// \brief Move constructor.
        ///
        /// Move-constructs a RGBColor instance, making it point at the same object that other
        /// was pointing to.
        ///
        /// \param other The moved color.
        constexpr RGBColor(RGBColor<UnderlyingType> &&other) noexcept = default;

        /// \brief Assigns other to this color and returns a reference to this color
        /// \param other The color copied.
        /// \returns The reference of the copied color.
        constexpr auto operator=(const RGBColor<UnderlyingType> &other) noexcept
            -> RGBColor      & = default;

        /// \brief Move assign other to this RGBColor instance.
        /// \param other The color moved.
        /// \returns The reference of the moved color.
        constexpr auto operator=(RGBColor<UnderlyingType> &&other) noexcept -> RGBColor & = default;

        /// \brief Construct a color from the convertion of an other color.
        ///
        /// Values are converted with convertColorComponent.
        ///
        /// \param other The color converted.
        /// \requires Type `U` shall be either `float` or
        /// [`UInt8`](standardese://UInt8)
        template<ColorComponentType U>
        constexpr RGBColor(const RGBColor<U> &other) noexcept;

        /// \brief Assigns and convert other to this color and returns a reference to this
        /// color.
        ///
        /// Values are converted with convertColorComponent
        ///
        /// \param other The color to convert.
        /// \returns The reference of the converted color.
        /// \requires Type `U` shall be either `float` or
        /// [`UInt8`](standardese://UInt8)
        template<ColorComponentType U>
        constexpr auto operator=(const RGBColor<U> &other) noexcept -> RGBColor &;

        /// \output_section Convertion members
        /// \brief Convert the color to a [`Vector3`](standardese://Vector3).
        /// \returns The converted [`Vector3`](standardese://Vector3).
        constexpr auto toVector3() const noexcept -> Vector3<UnderlyingType>;

        /// \brief Convert the color to a [`Vector4`](standardese://Vector4).
        /// \returns The converted [`Vector4`](standardese://Vector4).
        constexpr auto toVector4() const noexcept -> Vector4<UnderlyingType>;

        /// \brief Convert the color to a [`Vector3`](standardese://stormkit::core::Vector3).
        /// \returns The converted [`Vector3`](standardese://Vector3).
        constexpr operator Vector3<UnderlyingType>() const noexcept;

        /// \brief Convert the color to a [`Vector4`](standardese://Vector4).
        /// \returns The converted [`Vector4`](standardese://Vector4).
        constexpr operator Vector4<UnderlyingType>() const noexcept;

        /// \output_section Public attributes
        /// \brief Red attribute.
        UnderlyingType red;

        /// \brief Green attribute.
        UnderlyingType green;

        /// \brief Blue attribute.
        UnderlyingType blue;

        /// \brief Alpha attribute.
        UnderlyingType alpha;
    };

    /// \brief Alias of `RGBColor<UInt8>`.
    /// \unique_name RGBColorU
    using RGBColorU = RGBColor<UInt8>;

    /// \brief Alias of `RGBColor<float>`.
    /// \unique_name RGBColorF
    using RGBColorF = RGBColor<float>;

    /// This namespace contain preset colors, these colors are defined from [HTML 4.01
    /// colors](https://en.wikipedia.org/wiki/Web_colors#HTML_color_names).
    ///```
    /// | name        | red |green|blue |alpha|
    /// |-------------|-----|-----|-----|-----|
    /// | Black       | 0   | 0   | 0   | 255 |
    /// | Gray        | 127 | 127 | 127 | 255 |
    /// | Silver      | 190 | 190 | 190 | 255 |
    /// | White       | 255 | 255 | 255 | 255 |
    /// | Maroon      | 127 | 0   | 0   | 255 |
    /// | Red         | 255 | 0   | 0   | 255 |
    /// | Olive       | 127 | 127 | 0   | 255 |
    /// | Yellow      | 255 | 255 | 0   | 255 |
    /// | Green       | 0   | 127 | 0   | 255 |
    /// | Lime        | 0   | 255 | 0   | 255 |
    /// | Teal        | 0   | 127 | 127 | 255 |
    /// | Aqua        | 0   | 255 | 255 | 255 |
    /// | Navy        | 0   | 0   | 127 | 255 |
    /// | Blue        | 0   | 0   | 255 | 255 |
    /// | Purple      | 127 | 0   | 127 | 255 |
    /// | Fuschia     | 255 | 0   | 255 | 255 |
    /// | Transparent | 0   | 0   | 0   | 0   |
    ///```
    namespace RGBColorDef {
        template<ColorComponentType T>
        static constexpr auto Black =
            RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Gray = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Silver = RGBColor<T> {
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            maxColorComponentValue<T>()
        };

        template<ColorComponentType T>
        static constexpr auto White = RGBColor<T> { maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Maroon = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Red = RGBColor<T> { maxColorComponentValue<T>(),
                                                  T { 0 },
                                                  T { 0 },
                                                  maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Olive = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Yellow = RGBColor<T> { maxColorComponentValue<T>(),
                                                     maxColorComponentValue<T>(),
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };
        template<ColorComponentType T>
        static constexpr auto Green = RGBColor<T> { T { 0 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Lime = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   T { 0 },
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Teal = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Aqua = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Navy = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Blue = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Purple = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() / T { 2 },
                                                     maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Fuchsia = RGBColor<T> { maxColorComponentValue<T>(),
                                                      T { 0 },
                                                      maxColorComponentValue<T>(),
                                                      maxColorComponentValue<T>() };

        template<ColorComponentType T>
        static constexpr auto Transparent = RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, T { 0 } };
    } // namespace RGBColorDef
} // namespace stormkit::core

template<stormkit::core::ColorComponentType T>
struct std::hash<stormkit::core::RGBColor<T>> {
    [[nodiscard]] constexpr auto operator()(const stormkit::core::RGBColor<T> &color) const noexcept
        -> stormkit::core::Hash64;
};

template<stormkit::core::ColorComponentType T>
struct std::equal_to<stormkit::core::RGBColor<T>> {
    [[nodiscard]] constexpr auto
        operator()(const stormkit::core::RGBColor<T> &first,
                   const stormkit::core::RGBColor<T> &second) const noexcept -> bool;
};

#include "Color.inl"
