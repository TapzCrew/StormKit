// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/Expected.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::image {
    using XOffset = core::NamedType<core::UInt32, struct XOffsetTag, core::ArithmeticLike>;
    using YOffset = core::NamedType<core::UInt32, struct YOffsetTag, core::ArithmeticLike>;

    class STORMKIT_PUBLIC Image {
      public:
        enum class Format : core::UInt8 {
            R8_SNorm     = 0,
            RG8_SNorm    = 1,
            RGB8_SNorm   = 2,
            RGBA8_SNorm  = 3,
            R8_UNorm     = 4,
            RG8_UNorm    = 5,
            RGB8_UNorm   = 6,
            RGBA8_UNorm  = 7,
            R16_SNorm    = 8,
            RG16_SNorm   = 9,
            RGB16_SNorm  = 10,
            RGBA16_SNorm = 11,
            R16_UNorm    = 12,
            RG16_UNorm   = 13,
            RGB16_UNorm  = 14,
            RGBA16_UNorm = 15,
            RGBA4_UNorm  = 17,
            BGR8_UNorm   = 20,
            BGRA8_UNorm  = 21,
            R8I          = 22,
            RG8I         = 23,
            RGB8I        = 24,
            RGBA8I       = 25,
            R8U          = 26,
            RG8U         = 27,
            RGB8U        = 28,
            RGBA8U       = 29,
            R16I         = 30,
            RG16I        = 31,
            RGB16I       = 32,
            RGBA16I      = 33,
            R16U         = 34,
            RG16U        = 35,
            RGB16U       = 36,
            RGBA16U      = 37,
            R32I         = 38,
            RG32I        = 39,
            RGB32I       = 40,
            RGBA32I      = 41,
            R32U         = 42,
            RG32U        = 43,
            RGB32U       = 44,
            RGBA32U      = 45,
            R16F         = 47,
            RG16F        = 48,
            RGB16F       = 49,
            RGBA16F      = 50,
            R32F         = 51,
            RG32F        = 52,
            RGB32F       = 53,
            RGBA32F      = 54,
            sRGB8        = 56,
            sRGBA8       = 57,
            sBGR8        = 58,
            sBGRA8       = 59,
            Undefined    = 254
        };

        enum class Codec : std::uint8_t {
            Autodetect = 0,
            JPEG       = 1,
            PNG        = 2,
            TARGA      = 3,
            PPM        = 4,
            HDR        = 5,
            KTX        = 6,
            QOI        = 7,
            Unknown    = 255
        };

        enum class CodecArgs : std::uint8_t {
            Binary = 0,
            Ascii  = 1
        };

        struct Error {
            enum class Reason {
                Not_Implemented,
                Failed_To_Parse,
                Failed_To_Save,
                File_Not_Found,
                Invalid_Format
            } reason;

            std::string str_error;
        };

        struct ImageData {
            core::ExtentU extent           = { 0u, 0u, 0u };
            core::UInt32 channel_count     = 0u;
            core::UInt32 bytes_per_channel = 0u;
            core::UInt32 layers            = 1u;
            core::UInt32 faces             = 1u;
            core::UInt32 mip_levels        = 1u;
            Format format                  = Format::Undefined;

            core::ByteArray data = {};
        };

        Image() noexcept;
        explicit Image(ImageData&& data) noexcept;
        Image(const core::ExtentU& extent, Format format) noexcept;
        Image(const std::filesystem::path& filepath, Codec codec = Codec::Autodetect) noexcept;
        Image(core::ByteConstSpan data, Codec codec = Codec::Autodetect) noexcept;
        ~Image() noexcept;

        Image(const Image& rhs) noexcept;
        auto operator=(const Image& rhs) noexcept -> Image&;

        Image(Image&& rhs) noexcept;
        auto operator=(Image&& rhs) noexcept -> Image&;

        [[nodiscard]] auto loadFromFile(std::filesystem::path filepath,
                                        Codec codec = Codec::Autodetect) noexcept
            -> core::Expected<void, Error>;
        [[nodiscard]] auto loadFromMemory(core::ByteConstSpan data,
                                          Codec codec = Codec::Autodetect) noexcept
            -> core::Expected<void, Error>;
        [[nodiscard]] auto saveToFile(std::filesystem::path filename,
                                      Codec codec,
                                      CodecArgs args = CodecArgs::Binary) const noexcept
            -> core::Expected<void, Error>;

        [[nodiscard]] auto saveToMemory(Codec codec,
                                        CodecArgs args = CodecArgs::Binary) const noexcept
            -> core::Expected<core::ByteArray, Error>;

        auto create(core::ExtentU extent, Format format) noexcept -> void;

        [[nodiscard]] auto toFormat(Format format) const noexcept -> Image;
        [[nodiscard]] auto scale(const core::ExtentU& scale_to) const noexcept -> Image;
        [[nodiscard]] auto flipX() const noexcept -> Image;
        [[nodiscard]] auto flipY() const noexcept -> Image;
        [[nodiscard]] auto flipZ() const noexcept -> Image;
        [[nodiscard]] auto rotate90() const noexcept -> Image;
        [[nodiscard]] auto rotate180() const noexcept -> Image;
        [[nodiscard]] auto rotate270() const noexcept -> Image;

        [[nodiscard]] auto pixel(core::USize id,
                                 core::UInt32 layer = 0u,
                                 core::UInt32 face  = 0u,
                                 core::UInt32 level = 0u) noexcept -> core::ByteSpan;
        [[nodiscard]] auto pixel(core::USize id,
                                 core::UInt32 layer = 0u,
                                 core::UInt32 face  = 0u,
                                 core::UInt32 level = 0u) const noexcept -> core::ByteConstSpan;
        [[nodiscard]] auto pixel(core::Position3u position,
                                 core::UInt32 layer = 0u,
                                 core::UInt32 face  = 0u,
                                 core::UInt32 level = 0u) noexcept -> core::ByteSpan;
        [[nodiscard]] auto pixel(core::Position3u position,
                                 core::UInt32 layer = 0u,
                                 core::UInt32 face  = 0u,
                                 core::UInt32 level = 0u) const noexcept -> core::ByteConstSpan;

        [[nodiscard]] auto extent(core::UInt32 level = 0u) const noexcept -> core::ExtentU;

        [[nodiscard]] auto channelCount() const noexcept -> core::UInt32;

        [[nodiscard]] auto bytesPerChannel() const noexcept -> core::UInt32;

        [[nodiscard]] auto layers() const noexcept -> core::UInt32;

        [[nodiscard]] auto faces() const noexcept -> core::UInt32;

        [[nodiscard]] auto mipLevels() const noexcept -> core::UInt32;

        [[nodiscard]] auto format() const noexcept -> Format;

        [[nodiscard]] auto size() const noexcept -> core::USize;
        [[nodiscard]] auto size(core::UInt32 layer,
                                core::UInt32 face,
                                core::UInt32 level) const noexcept -> core::USize;
        [[nodiscard]] auto size(core::UInt32 layer, core::UInt32 face) const noexcept
            -> core::USize;
        [[nodiscard]] auto size(core::UInt32 layer) const noexcept -> core::USize;

        [[nodiscard]] auto data() noexcept -> core::ByteSpan;
        [[nodiscard]] auto data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept
            -> core::ByteSpan;
        [[nodiscard]] auto data() const noexcept -> core::ByteConstSpan;
        [[nodiscard]] auto data(core::UInt32 layer,
                                core::UInt32 face,
                                core::UInt32 level) const noexcept -> core::ByteConstSpan;

        [[nodiscard]] auto begin() noexcept;
        [[nodiscard]] auto
            begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept;
        [[nodiscard]] auto begin() const noexcept;
        [[nodiscard]] auto
            begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] auto cbegin() const noexcept;
        [[nodiscard]] auto
            cbegin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] auto end() noexcept;
        [[nodiscard]] auto end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept;
        [[nodiscard]] auto end() const noexcept;
        [[nodiscard]] auto
            end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] auto cend() const noexcept;
        [[nodiscard]] auto
            cend(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] auto imageData() const noexcept -> const ImageData&;

      private:
        ImageData m_data;
    };

    constexpr auto getChannelCountFor(Image::Format format) noexcept -> core::UInt8;
    constexpr auto getSizeof(Image::Format format) noexcept -> core::UInt8;
} // namespace stormkit::image

#include "Image.inl"
