// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <filesystem>

/////////// - StormKit::core - ///////////
#include <storm/core/Assert.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Fwd.hpp>

namespace storm::image {
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
            Unknown    = 255
        };
        enum class CodecArgs : std::uint8_t { Binary = 0, Ascii = 1 };

        explicit Image() noexcept;
        Image(core::Extentu extent, Format format) noexcept;
        Image(const std::filesystem::path &filepath, Codec codec = Codec::Autodetect) noexcept;
        Image(core::ByteConstSpan data, Codec codec = Codec::Autodetect) noexcept;
        ~Image() noexcept;

        Image(const Image &rhs) noexcept;
        Image &operator=(const Image &rhs) noexcept;

        Image(Image &&rhs) noexcept;
        Image &operator=(Image &&rhs) noexcept;

        bool loadFromFile(std::filesystem::path filepath, Codec codec = Codec::Autodetect) noexcept;
        bool loadFromMemory(core::ByteConstSpan data, Codec codec = Codec::Autodetect) noexcept;
        bool saveToFile(std::filesystem::path filename,
                        Codec codec,
                        CodecArgs args = CodecArgs::Binary) const noexcept;
        std::vector<core::Byte> saveToMemory(Codec codec,
                                             CodecArgs args = CodecArgs::Binary) const noexcept;

        void create(core::Extentu extent, Format format) noexcept;

        Image toFormat(Format format) const noexcept;
        Image scale(const core::Extentu &scale_to) const noexcept;
        Image flipX() const noexcept;
        Image flipY() const noexcept;
        Image flipZ() const noexcept;
        Image rotate90() const noexcept;
        Image rotate180() const noexcept;
        Image rotate270() const noexcept;

        [[nodiscard]] inline core::ByteSpan pixel(core::ArraySize id,
                                                  core::UInt32 layer = 0u,
                                                  core::UInt32 face  = 0u,
                                                  core::UInt32 level = 0u) noexcept;
        [[nodiscard]] inline core::ByteConstSpan pixel(core::ArraySize id,
                                                       core::UInt32 layer = 0u,
                                                       core::UInt32 face  = 0u,
                                                       core::UInt32 level = 0u) const noexcept;
        [[nodiscard]] inline core::ByteSpan pixel(core::Position3u position,
                                                  core::UInt32 layer = 0u,
                                                  core::UInt32 face  = 0u,
                                                  core::UInt32 level = 0u) noexcept;
        [[nodiscard]] inline core::ByteConstSpan pixel(core::Position3u position,
                                                       core::UInt32 layer = 0u,
                                                       core::UInt32 face  = 0u,
                                                       core::UInt32 level = 0u) const noexcept;

        [[nodiscard]] inline core::Extentu extent(core::UInt32 level = 0u) const noexcept;
        [[nodiscard]] inline core::UInt32 channelCount() const noexcept;
        [[nodiscard]] inline core::UInt32 bytesPerChannel() const noexcept;
        [[nodiscard]] inline core::UInt32 layers() const noexcept;
        [[nodiscard]] inline core::UInt32 faces() const noexcept;
        [[nodiscard]] inline core::UInt32 mipLevels() const noexcept;
        [[nodiscard]] inline Format format() const noexcept;

        [[nodiscard]] inline core::ArraySize size() const noexcept;
        [[nodiscard]] inline core::ArraySize
            size(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;
        [[nodiscard]] inline core::ArraySize size(core::UInt32 layer,
                                                  core::UInt32 face) const noexcept;
        [[nodiscard]] inline core::ArraySize size(core::UInt32 layer) const noexcept;
        [[nodiscard]] inline core::ByteSpan data() noexcept;
        [[nodiscard]] inline core::ByteSpan
            data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept;
        [[nodiscard]] inline core::ByteConstSpan data() const noexcept;
        [[nodiscard]] inline core::ByteConstSpan
            data(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] inline core::ByteArray::iterator begin() noexcept;
        [[nodiscard]] inline core::ByteSpan::iterator
            begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator begin() const noexcept;
        [[nodiscard]] inline core::ByteConstSpan::iterator
            begin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator cbegin() const noexcept;
        [[nodiscard]] inline core::ByteConstSpan::iterator
            cbegin(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

        [[nodiscard]] inline core::ByteArray::iterator end() noexcept;
        [[nodiscard]] inline core::ByteSpan::iterator
            end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator end() const noexcept;
        [[nodiscard]] inline core::ByteConstSpan::iterator
            end(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;
        [[nodiscard]] inline core::ByteArray::const_iterator cend() const noexcept;
        [[nodiscard]] inline core::ByteConstSpan::iterator
            cend(core::UInt32 layer, core::UInt32 face, core::UInt32 level) const noexcept;

      private:
        std::optional<std::string> loadJPEG(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadPNG(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadTARGA(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadPPM(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadHDR(core::ByteConstSpan data) noexcept;
        std::optional<std::string> loadKTX(core::ByteConstSpan data) noexcept;

        std::optional<std::string> saveJPEG(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveTGA(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> savePNG(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> savePPM(const std::filesystem::path &filepath,
                                           CodecArgs args) const noexcept;
        std::optional<std::string> saveTARGA(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveHDR(const std::filesystem::path &filepath) const noexcept;
        std::optional<std::string> saveKTX(const std::filesystem::path &filepath) const noexcept;

        std::optional<std::string> saveJPEG(core::ByteArray &output) const noexcept;
        std::optional<std::string> saveTGA(core::ByteArray &output) const noexcept;
        std::optional<std::string> savePNG(core::ByteArray &output) const noexcept;
        std::optional<std::string> savePPM(core::ByteArray &output,
                                           CodecArgs args) const noexcept;
        std::optional<std::string> saveTARGA(core::ByteArray &output) const noexcept;
        std::optional<std::string> saveHDR(core::ByteArray &output) const noexcept;
        std::optional<std::string> saveKTX(core::ByteArray &output) const noexcept;

        core::Extentu m_extent           = { 0u, 0u, 0u };
        core::UInt32 m_channel_count     = 0u;
        core::UInt32 m_bytes_per_channel = 0u;
        core::UInt32 m_layers            = 1u;
        core::UInt32 m_faces             = 1u;
        core::UInt32 m_mip_levels        = 1u;
        Format m_format                  = Format::Undefined;

        core::ByteArray m_data;
    };

    STORMKIT_PUBLIC constexpr core::UInt8 getChannelCountFor(Image::Format format) noexcept;
    STORMKIT_PUBLIC constexpr core::UInt8 getArraySizeByChannelFor(Image::Format format) noexcept;
} // namespace storm::image

#include "Image.inl"
