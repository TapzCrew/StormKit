// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <type_traits>

/////////// - StormKit::core - ///////////
#include <storm/core/Fwd.hpp>
#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Types.hpp>

namespace storm::core {
    /// \brief Convert a color component from [`UInt8`](standardese://UInt8/) to
    /// `float`.
    ///
    /// The component will be remaped from [0, 255] to [0, 1.f].
    ///
    /// \param component The component to convert.
    /// \returns The converted component.
    /// \unique_name convertColorComponent(UInt8)
    constexpr float convertColorComponent(UInt8 component) noexcept;

    /// \brief Convert a color component from `float` to
    /// [`UInt8`](standardese://UInt8).
    ///
    /// The component will be remaped from [0, 1.f] to [0, 255].
    ///
    /// \param component The component to convert.
    /// \returns The converted component.
    /// \unique_name convertColorComponent(float)
    constexpr UInt8 convertColorComponent(float component) noexcept;

    /// \brief Get the max value of a component with from a given Type.
    /// \returns The maximum value of `T` color component
    /// \unique_name maxColorComponentValue<T>()
    template<COLOR_COMPONENT_CONCEPT(T)>
    constexpr T maxColorComponentValue() noexcept;

    /// \brief `RGBColor` is a representation of a RGB/RGBA color.
    ///
    /// This class can use as component storage type `float` and
    /// [`UInt8`](standardese://UInt8).
    /// \requires Type `T` shall be either `float` or [`UInt8`](standardese://UInt8).
    /// \unique_name RGBColor<T>
    template<COLOR_COMPONENT_CONCEPT(T)>
    struct RGBColor {
        /// \output_section Public Special Member Functions
        /// \brief Construct a color from red, gree, blue and optionaly alpha.
        /// \param red The amound of red.
        /// \param green The amound of green.
        /// \param blue The amound of blue.
        /// \param alpha The amound of alpha (default constructed to the `maxColorComponentValue` of
        /// the given T type).
        constexpr RGBColor(T red, T green, T blue, T alpha = maxColorComponentValue<T>()) noexcept;

        /// \brief Construct a color from a [`Vector3`](standardese://Vector3).
        /// \param vector The input [`Vector3`](standardese://Vector3).
        /// \notes The alpha value is constructed from `maxColorComponentValue`.
        constexpr explicit RGBColor(const Vector3<T> &vector) noexcept;

        /// \brief Construct a color from a [`Vector4`](standardese://Vector4).
        /// \param vector The input [`Vector4`](standardese://Vector4).
        constexpr explicit RGBColor(const Vector4<T> &vector) noexcept;

        /// \brief Copy constructor.
        ///
        /// Construct a copy of other.
        ///
        /// \param other The copied color.
        constexpr RGBColor(const RGBColor<T> &other) noexcept = default;

        /// \brief Move constructor.
        ///
        /// Move-constructs a RGBColor instance, making it point at the same object that other was
        /// pointing to.
        ///
        /// \param other The moved color.
        constexpr RGBColor(RGBColor<T> &&other) noexcept = default;

        /// \brief Assigns other to this color and returns a reference to this color
        /// \param other The color copied.
        /// \returns The reference of the copied color.
        constexpr RGBColor &operator=(const RGBColor<T> &other) noexcept = default;

        /// \brief Move assign other to this RGBColor instance.
        /// \param other The color moved.
        /// \returns The reference of the moved color.
        constexpr RGBColor &operator=(RGBColor<T> &&other) noexcept = default;

        /// \brief Construct a color from the convertion of an other color.
        ///
        /// Values are converted with convertColorComponent.
        ///
        /// \param other The color converted.
        /// \requires Type `U` shall be either `float` or
        /// [`UInt8`](standardese://UInt8)
        template<COLOR_COMPONENT_CONCEPT(U)>
        constexpr RGBColor(const RGBColor<U> &other) noexcept;

        /// \brief Assigns and convert other to this color and returns a reference to this color.
        ///
        /// Values are converted with convertColorComponent
        ///
        /// \param other The color to convert.
        /// \returns The reference of the converted color.
        /// \requires Type `U` shall be either `float` or
        /// [`UInt8`](standardese://UInt8)
        template<COLOR_COMPONENT_CONCEPT(U)>
        constexpr RGBColor &operator=(const RGBColor<U> &other) noexcept;

        /// \output_section Convertion members
        /// \brief Convert the color to a [`Vector3`](standardese://Vector3).
        /// \returns The converted [`Vector3`](standardese://Vector3).
        constexpr Vector3<T> toVector3() const noexcept;

        /// \brief Convert the color to a [`Vector4`](standardese://Vector4).
        /// \returns The converted [`Vector4`](standardese://Vector4).
        constexpr Vector4<T> toVector4() const noexcept;

        /// \brief Convert the color to a [`Vector3`](standardese://storm::core::Vector3).
        /// \returns The converted [`Vector3`](standardese://Vector3).
        constexpr operator Vector3<T>() const noexcept;

        /// \brief Convert the color to a [`Vector4`](standardese://Vector4).
        /// \returns The converted [`Vector4`](standardese://Vector4).
        constexpr operator Vector4<T>() const noexcept;

        /// \output_section Public attributes
        /// \brief Red attribute.
        T red;

        /// \brief Green attribute.
        T green;

        /// \brief Blue attribute.
        T blue;

        /// \brief Alpha attribute.
        T alpha;
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
        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Black =
            RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Gray = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Silver = RGBColor<T> {
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            (maxColorComponentValue<T>() / T { 2 }) + (maxColorComponentValue<T>() / T { 4 }),
            maxColorComponentValue<T>()
        };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto White = RGBColor<T> { maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>(),
                                                    maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Maroon = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Red = RGBColor<T> { maxColorComponentValue<T>(),
                                                  T { 0 },
                                                  T { 0 },
                                                  maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Olive = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Yellow = RGBColor<T> { maxColorComponentValue<T>(),
                                                     maxColorComponentValue<T>(),
                                                     T { 0 },
                                                     maxColorComponentValue<T>() };
        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Green = RGBColor<T> { T { 0 },
                                                    maxColorComponentValue<T>() / T { 2 },
                                                    T { 0 },
                                                    maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Lime = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   T { 0 },
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Teal = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Aqua = RGBColor<T> { T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Navy = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>() / T { 2 },
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Blue = RGBColor<T> { T { 0 },
                                                   T { 0 },
                                                   maxColorComponentValue<T>(),
                                                   maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Purple = RGBColor<T> { maxColorComponentValue<T>() / T { 2 },
                                                     T { 0 },
                                                     maxColorComponentValue<T>() / T { 2 },
                                                     maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Fuchsia = RGBColor<T> { maxColorComponentValue<T>(),
                                                      T { 0 },
                                                      maxColorComponentValue<T>(),
                                                      maxColorComponentValue<T>() };

        template<COLOR_COMPONENT_CONCEPT(T)>
        static constexpr auto Transparent = RGBColor<T> { T { 0 }, T { 0 }, T { 0 }, T { 0 } };
    } // namespace RGBColorDef
} // namespace storm::core

namespace std {
    template<COLOR_COMPONENT_CONCEPT(T)>
    struct hash<storm::core::RGBColor<T>> {
        [[nodiscard]] constexpr storm::core::Hash64
            operator()(const storm::core::RGBColor<T> &color) const noexcept {
            auto hash = storm::core::Hash64 {};
            storm::core::hashCombine(hash, color.red);
            storm::core::hashCombine(hash, color.green);
            storm::core::hashCombine(hash, color.blue);
            storm::core::hashCombine(hash, color.alpha);

            return hash;
        }
    };

    template<COLOR_COMPONENT_CONCEPT(T)>
    struct equal_to<storm::core::RGBColor<T>> {
        [[nodiscard]] constexpr bool
            operator()(const storm::core::RGBColor<T> &first,
                       const storm::core::RGBColor<T> &second) const noexcept {
            const auto hasher = hash<storm::core::RGBColor<T>> {};
            return hasher(first) == hasher(second);
        }
    };
} // namespace std

#include "Color.inl"
