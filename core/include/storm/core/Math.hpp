// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <variant>

/////////// - GSL - ///////////
#include <gsl/string_span>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NamedType.hpp>
#include <storm/core/Format.hpp>

/////////// - GLM - ///////////
/// \exclude
#define GLM_FORCE_RADIANS

/// \exclude
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_integer.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace storm::core {
    using namespace glm;

    /// \exclude
    namespace details {
        struct Position2fTag;
        struct Position2uTag;
        struct Position2iTag;
        struct Position3fTag;
        struct Position3uTag;
        struct Position3iTag;

        struct MoveOffset2fTag;
        struct MoveOffset2uTag;
        struct MoveOffset2iTag;
        struct MoveExtentfOffsetTag;
        struct MoveExtentuOffsetTag;
        struct MoveOffsetTag;

        struct Scale2fTag;
        struct Scale2uTag;
        struct Scale2iTag;
        struct Scale3fTag;
        struct Scale3uTag;
        struct Scale3iTag;

        struct Orientation2fTag;
        struct Orientation2uTag;
        struct Orientation2iTag;
        struct Orientation3fTag;
        struct Orientation3uTag;
        struct Orientation3iTag;

        struct Rotation2fTag;
        struct Rotation2uTag;
        struct Rotation2iTag;
        struct Rotation3fTag;
        struct Rotation3uTag;
        struct Rotation3iTag;

        template<typename T>
        struct ExtentOffsetTag;
    } // namespace details

    /// \group vector-types Math vector types.
    ///
    /// Aliases to `glm::vec*` types.
    ///
    /// \unique_name Vector2<T>
    template<typename T>
    using Vector2 = glm::vec<2, T, glm::defaultp>;

    /// \group vector-types
    /// \unique_name Vector3<T>
    template<typename T>
    using Vector3 = glm::vec<3, T, glm::defaultp>;

    /// \group vector-types
    /// \unique_name Vector4<T>
    template<typename T>
    using Vector4 = glm::vec<4, T, glm::defaultp>;

    /// \group vector-types
    /// \unique_name Vector2f
    using Vector2f = glm::vec2;

    /// \group vector-types
    /// \unique_name Vector3f
    using Vector3f = glm::vec3;

    /// \group vector-types
    /// \unique_name Vector4f
    using Vector4f = glm::vec4;

    /// \group vector-types
    /// \unique_name Vector2u
    using Vector2u = glm::uvec2;

    /// \group vector-types
    /// \unique_name Vector3u
    using Vector3u = glm::uvec3;

    /// \group vector-types
    /// \unique_name Vector4u
    using Vector4u = glm::uvec4;

    /// \group vector-types
    /// \unique_name Vector2i
    using Vector2i = glm::ivec2;

    /// \group vector-types
    /// \unique_name Vector3i
    using Vector3i = glm::ivec3;

    /// \group vector-types
    /// \unique_name Vector4i
    using Vector4i = glm::ivec4;

    /// \group matrices-types Math matrices types.
    ///
    /// Aliases to `glm::mat*`.
    ///
    /// \unique_name Matrix<T>
    template<typename T>
    using MatrixBase = glm::mat<4, 4, T, glm::defaultp>;

    /// \group matrices-types
    /// \unique_name Matrixu
    using Matrixu = glm::umat4x4;

    /// \group matrices-types
    /// \unique_name Matrixi
    using Matrixi = glm::imat4x4;

    /// \group matrices-types
    /// \unique_name Matrixf
    using Matrixf = glm::mat4;
    using Matrix  = glm::mat4;

    /// \brief Quaternion.
    ///
    /// Alias to `glm::quat`.
    ///
    /// \unique_name Quaternion
    using Quaternion = glm::quat;

    /// \group position-types Position types
    ///
    /// Strongly typed vector type that represent a position.
    ///
    /// \unique_name Position2f
    using Position2f = core::NamedType<Vector2f, details::Position2fTag>;

    /// \group position-types
    /// \unique_name Position2u
    using Position2u = core::NamedType<Vector2u, details::Position2uTag>;

    /// \group position-types
    /// \unique_name Position2i
    using Position2i = core::NamedType<Vector2i, details::Position2iTag>;

    /// \group position-types
    /// \unique_name Position3f
    using Position3f = core::NamedType<Vector3f, details::Position3fTag>;

    /// \group position-types
    /// \unique_name Position3u
    using Position3u = core::NamedType<Vector3u, details::Position3uTag>;

    /// \group position-types
    /// \unique_name Position3i
    using Position3i = core::NamedType<Vector3i, details::Position3iTag>;

    /// \group move-offset-types Move offset types
    ///
    /// Strongly typed vector type that represent a move offset.
    ///
    /// \unique_name MoveOffset2f
    using MoveOffset2f = core::NamedType<Vector2f, details::MoveOffset2fTag>;

    /// \group move-offset-types
    /// \unique_name MoveOffset2u
    using MoveOffset2u = core::NamedType<Vector2u, details::MoveOffset2uTag>;

    /// \group move-offset-types
    /// \unique_name MoveOffset2i
    using MoveOffset2i = core::NamedType<Vector2i, details::MoveOffset2iTag>;

    /// \group move-offset-types
    /// \unique_name MoveExtentfOffset
    using MoveExtentfOffset = core::NamedType<Vector3f, details::MoveExtentfOffsetTag>;

    /// \group move-offset-types
    /// \unique_name MoveExtentuOffset
    using MoveExtentuOffset = core::NamedType<Vector3u, details::MoveExtentuOffsetTag>;

    /// \group move-offset-types
    /// \unique_name MoveOffset
    using MoveOffset = core::NamedType<Vector3i, details::MoveOffsetTag>;

    /// \group scale-types Scale types
    ///
    /// Strongly typed vector type that represent a scale.
    ///
    /// \unique_name Scale2f
    using Scale2f = core::NamedType<Vector2f, details::Scale2fTag>;

    /// \group scale-types
    /// \unique_name Scale2u
    using Scale2u = core::NamedType<Vector2u, details::Scale2uTag>;

    /// \group scale-types
    /// \unique_name Scale2i
    using Scale2i = core::NamedType<Vector2i, details::Scale2iTag>;

    /// \group scale-types
    /// \unique_name Scale3f
    using Scale3f = core::NamedType<Vector3f, details::Scale3fTag>;

    /// \group scale-types
    /// \unique_name Scale3u
    using Scale3u = core::NamedType<Vector3u, details::Scale3uTag>;

    /// \group scale-types
    /// \unique_name Scale3i
    using Scale3i = core::NamedType<Vector3i, details::Scale3iTag>;

    /// \group orientation-types Orientation types
    ///
    /// Strongly typed vector type that represent an orientation.
    ///
    /// \unique_name Orientation2f
    using Orientation2f = core::NamedType<Vector2f, details::Orientation2fTag>;

    /// \group orientation-types
    /// \unique_name Orientation2u
    using Orientation2u = core::NamedType<Vector2u, details::Orientation2uTag>;

    /// \group orientation-types
    /// \unique_name Orientation2i
    using Orientation2i = core::NamedType<Vector2i, details::Orientation2iTag>;

    /// \group orientation-types
    /// \unique_name Orientation3f
    using Orientation3f = core::NamedType<Vector3f, details::Orientation3fTag>;

    /// \group orientation-types
    /// \unique_name Orientation3u
    using Orientation3u = core::NamedType<Vector3u, details::Orientation3uTag>;

    /// \group orientation-types
    /// \unique_name Orientation3i
    using Orientation3i = core::NamedType<Vector3i, details::Orientation3iTag>;

    /// \group rotation-types Rotation types
    ///
    /// Strongly typed vector type that represent a rotation.
    ///
    /// \unique_name Rotation2f
    using Rotation2f = core::NamedType<Vector2f, details::Rotation2fTag>;

    /// \group rotation-types
    /// \unique_name Rotation2u
    using Rotation2u = core::NamedType<Vector2u, details::Rotation2uTag>;

    /// \group rotation-types
    /// \unique_name Rotation2i
    using Rotation2i = core::NamedType<Vector2i, details::Rotation2iTag>;

    /// \group rotation-types
    /// \unique_name Rotation3f
    using Rotation3f = core::NamedType<Vector3f, details::Rotation3fTag>;

    /// \group rotation-types
    /// \unique_name Rotation3u
    using Rotation3u = core::NamedType<Vector3u, details::Rotation3uTag>;

    /// \group rotation-types
    /// \unique_name Rotation3i
    using Rotation3i = core::NamedType<Vector3i, details::Rotation3iTag>;

    /// \brief Representation of an extent.
    /// \requires `T` need to be an arithmetic type (satisfy std::is_floating_point or
    /// std::is_integral).
    /// \unique_name ExtentBase<T>
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    struct Extent {
        /// \output_section Public Special Member Functions
        /// \brief Default construct an extent.
        ///
        /// `width` `height` and `depth` are default constructed to 1.
        constexpr Extent() noexcept = default;

        /// \brief Construct an extent from width, height and optionnaly depth
        /// \param red The amound of width.
        /// \param green The amound of height.
        constexpr Extent(T width, T height, T depth = T{1}) noexcept;

        /// \brief Copy constructor.
        ///
        /// Construct a copy of other.
        ///
        /// \param other The copied extent.
        constexpr Extent(const Extent<T> &other) noexcept = default;

        /// \brief Move constructor.
        ///
        /// Move-constructs a `Extent` instance, making it point at the same object that other was
        /// pointing to.
        ///
        /// \param other The moved extent.
        constexpr Extent(Extent<T> &&other) noexcept = default;

        /// \brief Assigns other to this extent and returns a reference to this extent.
        /// \param other The extent copied.
        /// \returns The reference of the copied extent.
        constexpr Extent<T> &operator=(const Extent<T> &other) noexcept = default;

        /// \brief Move assign other to this Extent instance.
        /// \param other The extent moved.
        /// \returns The reference of the moved extent.
        constexpr Extent<T> &operator=(Extent<T> &&other) noexcept = default;

        /// \brief Construct a extent from the convertion of an other extent.
        ///
        /// Values are converted with static_cast.
        ///
        /// \param other The extent converted.
        /// \requires `U` need to be an arithmetic type (satisfy std::is_floating_point or
        /// std::is_integral).
        template<ARITHMETIC_TYPE_CONCEPT(U)>
        constexpr Extent(const Extent<U> &other) noexcept;

        /// \output_section Publics operators members
        /// \brief Test the equality with an other extent.
        /// \param other The extent to test
        /// \returns true if this extent is equal to `other`, otherwise returns false.
        [[nodiscard]] constexpr bool operator==(const Extent<T> &other) const noexcept;

        /// \brief Multiply an extent with a factor.
        /// \param factor The factor to multiply
        /// \returns A newly constructed extent equal to this extent multiplied with `factor`
        [[nodiscard]] constexpr Extent<T> operator*(T factor) const noexcept;

        /// \brief Divide an extent with a factor.
        /// \param factor The factor to divide
        /// \returns A newly constructed extent equal to this extent Divided with `factor`
        [[nodiscard]] constexpr Extent<T> operator/(T factor) const noexcept;

        /// \brief Multiply this extent with a factor.
        /// \param factor The factor to multiply
        /// \returns A reference to this after the multiplication with `factor`
        [[nodiscard]] constexpr Extent<T> &operator*=(T factor) noexcept;

        /// \brief Divide this extent with a factor.
        /// \param factor The factor to divide
        /// \returns A reference to this after the division with `factor`
        [[nodiscard]] constexpr Extent<T> &operator/=(T factor) noexcept;

        /// \output_section Publics member
        /// \brief Width attribute.
        T width = 0;

        /// \brief Height attribute.
        T height = 0;

        /// \brief Depth attribute.
        T depth = 1;
    };

    /// \brief Alias of `Extent<Int32>`.
    /// \unique_name Extenti
    using Extenti = Extent<core::Int32>;

    /// \brief Alias of `Extent<UInt32>`.
    /// \unique_name Extentu
    using Extentu = Extent<core::UInt32>;

    /// \brief Alias of `Extent<float>`.
    /// \unique_name Extentf
    using Extentf = Extent<float>;

    /// \group offset-types Offset types
    ///
    /// Strongly typed extent type that represent an Extent offset.
    ///
    /// \unique_name Offset
    template<typename T>
    using ExtentOffset = core::NamedType<Extent<T>, details::ExtentOffsetTag<T>>;

    /// \group offset-types
    /// \unique_name Offseti
    using ExtentiOffset = ExtentOffset<core::Int32>;

    /// \group offset-types
    /// \unique_name Offseti
    using ExtentuOffset = ExtentOffset<core::UInt32>;

    /// \group offset-types
    /// \unique_name Offseti
    using ExtentfOffset = ExtentOffset<float>;

FORMATTER(storm::core::Vector2f, "Vector2 {{ .x = {}, .y = {} }}", data.x, data.y)
FORMATTER(storm::core::Position2f, "Position2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::MoveOffset2f, "MoveOffset2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Scale2f, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Orientation2f, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Rotation2f, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

FORMATTER(storm::core::Vector3f,
              "Vector3 {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
FORMATTER(storm::core::Position3f,
              "Position3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::MoveExtentfOffset,
              "MoveOffset3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Scale3f,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Orientation3f,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Rotation3f,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

FORMATTER(storm::core::Vector4f,
              "Vector4 {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

FORMATTER(storm::core::Vector2u, "Vector2u {{ .x = {}, .y = {} }}", data.x, data.y)
FORMATTER(storm::core::Position2u, "Position2u {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::MoveOffset2u, "MoveInt322 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Scale2u, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Orientation2u, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Rotation2u, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

FORMATTER(storm::core::Vector3u,
              "Vector3u {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
FORMATTER(storm::core::Position3u, "Position3u {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::MoveExtentuOffset,
              "MoveInt323 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Scale3u,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Orientation3u,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Rotation3u,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

FORMATTER(storm::core::Vector4u,
              "Vector4u {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

FORMATTER(storm::core::Vector2i, "Vector2i {{ .x = {}, .y = {} }}", data.x, data.y)
FORMATTER(storm::core::Position2i, "Position2i {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::MoveOffset2i, "MoveOffset2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Scale2i, "Scale2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Orientation2i, "Orientation2 {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::Rotation2i, "Rotation2 {{ .x = {}, .y = {} }}", data->x, data->y)

FORMATTER(storm::core::Vector3i,
              "Vector3i {{ .x = {}, .y = {}, .z = {} }}",
              data.x,
              data.y,
              data.z)
FORMATTER(storm::core::Position3i, "Position3i {{ .x = {}, .y = {} }}", data->x, data->y)
FORMATTER(storm::core::MoveOffset,
              "MoveOffset3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Scale3i,
              "Scale3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Orientation3i,
              "Orientation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)
FORMATTER(storm::core::Rotation3i,
              "Rotation3 {{ .x = {}, .y = {}, .z = {} }}",
              data->x,
              data->y,
              data->z)

FORMATTER(storm::core::Vector4i,
              "Vector4i {{ .x = {}, .y = {}, .z = {}, .w = {} }}",
              data.x,
              data.y,
              data.z,
              data.w)

FORMATTER(storm::core::Matrix,
              "Matrix {{\n    {},\n    {},\n    {},\n    {}\n}}",
              data[0],
              data[1],
              data[2],
              data[3])

FORMATTER(storm::core::Extenti,
              "Extenti {{ .width = {}, .height = {}, .depth = {} }}",
              data.width,
              data.height,
              data.depth)
FORMATTER(storm::core::Extentu,
              "Extentu {{ .width = {}, .height = {}, .depth = {} }}",
              data.width,
              data.height,
              data.depth)
FORMATTER(storm::core::Extentf,
              "Extentf {{ .width = {}, .height = {}. depth = {} }}",
              data.width,
              data.height,
              data.depth)
} // namespace storm::core

namespace std {
    template<ARITHMETIC_TYPE_CONCEPT(T)>
    struct hash<storm::core::Extent<T>> {
        [[nodiscard]] constexpr storm::core::Hash64
            operator()(const storm::core::Extent<T> &extent) const noexcept {
            auto hash = storm::core::Hash64 { 0 };
            storm::core::hashCombine(hash, extent.width);
            storm::core::hashCombine(hash, extent.height);
            storm::core::hashCombine(hash, extent.depth);

            return hash;
        }
    };
} // namespace std

CUSTOM_FORMAT(storm::core::Vector2f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position2f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveOffset2f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale2f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation2f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation2f, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector3f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position3f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveExtentfOffset, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale3f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation3f, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation3f, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector4f, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector2u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position2u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveOffset2u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale2u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation2u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation2u, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector3u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position3u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveExtentuOffset, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale3u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation3u, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation3u, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector4u, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector2i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position2i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveOffset2i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale2i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation2i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation2i, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector3i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Position3i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::MoveOffset, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Scale3i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Orientation3i, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Rotation3i, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Vector4i, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Matrix, storm::core::toString(data))

CUSTOM_FORMAT(storm::core::Extenti, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Extentu, storm::core::toString(data))
CUSTOM_FORMAT(storm::core::Extentf, storm::core::toString(data))

#include "Math.inl"
