// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/Color.hpp>
#include <stormkit/core/FrozenMap.hpp>
#include <stormkit/core/Fstream.hpp>

#include "QOIImage.hpp"
#include "stormkit/core/Types.hpp"

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;

    struct QOIHeader {
        std::array<core::Byte, 4> magic;
        core::UInt32 width;
        core::UInt32 height;
        core::UInt8 channels;
        core::UInt8 colorspace;
    };

    constexpr auto SIZE_OF_HEADER = 14;

    static constexpr auto CHANNELS_TO_FORMAT =
        core::makeFrozenMap<core::Int32, std::array<image::Image::Format, 2>>(
            { { 3, std::array { image::Image::Format::sRGB8, image::Image::Format::RGB8_UNorm } },
              { 4,
                std::array { image::Image::Format::sRGBA8, image::Image::Format::RGBA8_UNorm } } });

    static constexpr auto END_OF_FILE =
        core::makeStaticByteArray(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);

    static constexpr auto PIXEL_CACHE_SIZE = 64u;

    enum class QOI_OPERATION : core::UInt8 {
        RGB   = 0b11111110,
        RGBA  = 0b11111111,
        INDEX = 0b00000000,
        DIFF  = 0b01000000,
        LUMA  = 0b10000000,
        RUN   = 0b11000000,
    };

    union Pixel {
        struct {
            core::UInt8 r = 0;
            core::UInt8 g = 0;
            core::UInt8 b = 0;
            core::UInt8 a = 0;
        } rgba;

        std::array<core::UInt8, 4> data;
    };

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto indexHash(const Pixel& pixel) noexcept {
        return (pixel.rgba.r * 3u + pixel.rgba.g * 5u + pixel.rgba.b * 7u + pixel.rgba.a * 11u) %
               PIXEL_CACHE_SIZE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadQOI([[maybe_unused]] core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        const auto raw_header = core::ByteConstSpan { std::data(data), SIZE_OF_HEADER };
        const auto *header    = reinterpret_cast<const QOIHeader *>(std::data(raw_header));

        const auto extent =
            core::ExtentU { core::byteSwap(header->width), core::byteSwap(header->height) };
        const auto channels = header->channels;
        const auto format   = CHANNELS_TO_FORMAT.at(header->channels)[header->colorspace];

        auto pixel_cache = std::array<Pixel, PIXEL_CACHE_SIZE> {};

        const auto chunks =
            std::span { reinterpret_cast<const core::UInt8 *>(std::data(data)) + SIZE_OF_HEADER,
                        std::size(data) - SIZE_OF_HEADER };

        const auto output_size = extent.width * extent.height * channels;

        auto output = core::ByteArray {};
        output.reserve(output_size);

        auto previous_pixel = Pixel { .rgba = { .a = 255 } };

        auto run = 0;

        const auto diff = 4 - channels;
        auto it         = std::ranges::begin(chunks);

        const auto chunks_size = output_size - std::size(END_OF_FILE);
        for ([[maybe_unused]] auto i : core::range(output_size, channels)) {
            const auto tag = *it;

            const auto position =
                core::as<core::USize>(std::distance(std::ranges::begin(chunks), it));

            if (run > 0) --run;
            else if (std::memcmp(&*it, std::data(END_OF_FILE), std::size(END_OF_FILE)) == 0)
                [[unlikely]] {
                it = std::ranges::cend(chunks);
            } else if (position < chunks_size) {
                ++it;
                if (static_cast<QOI_OPERATION>(tag) == QOI_OPERATION::RGB) {
                    previous_pixel.rgba.r = *it;
                    previous_pixel.rgba.g = *(it + 1);
                    previous_pixel.rgba.b = *(it + 2);

                    it += 3;
                } else if (static_cast<QOI_OPERATION>(tag) == QOI_OPERATION::RGBA) {
                    previous_pixel.rgba.r = *it;
                    previous_pixel.rgba.g = *(it + 1);
                    previous_pixel.rgba.b = *(it + 2);
                    previous_pixel.rgba.a = *(it + 3);

                    it += 4;
                } else {
#define CHECK(op) (tag & 0b11000000) == static_cast<core::UInt8>(op)
                    if (CHECK(QOI_OPERATION::INDEX)) {
                        const auto index = tag;

                        previous_pixel = pixel_cache[index];
                    } else if (CHECK(QOI_OPERATION::DIFF)) {
                        const auto r_diff = core::as<core::UInt8>(((tag >> 4) & 0x03) - 2);
                        const auto g_diff = core::as<core::UInt8>(((tag >> 2) & 0x03) - 2);
                        const auto b_diff = core::as<core::UInt8>((tag & 0x03) - 2);

                        previous_pixel.rgba.r += r_diff;
                        previous_pixel.rgba.g += g_diff;
                        previous_pixel.rgba.b += b_diff;

                    } else if (CHECK(QOI_OPERATION::LUMA)) {
                        const auto g_diff = (tag & 0x3f) - 32;

                        const auto current_r = ((*it) >> 4) & 0x0f;
                        const auto current_b = (*it) & 0x0f;

                        previous_pixel.rgba.r += core::as<core::UInt8>(g_diff - 8 + current_r);
                        previous_pixel.rgba.g += core::as<core::UInt8>(g_diff);
                        previous_pixel.rgba.b += core::as<core::UInt8>(g_diff - 8 + current_b);

                        ++it;
                    } else if (CHECK(QOI_OPERATION::RUN)) {
                        run = (tag & 0x3f);
                    }

#undef CHECK
                }

                auto& cached = pixel_cache[indexHash(previous_pixel)];
                cached       = previous_pixel;
            }

            std::ranges::transform(std::begin(previous_pixel.data),
                                   std::end(previous_pixel.data) - diff,
                                   std::back_inserter(output),
                                   [](const auto& b) { return core::as<core::Byte>(b); });
        }

        auto image_data = image::Image::ImageData { .extent            = extent,
                                                    .channel_count     = channels,
                                                    .bytes_per_channel = getSizeof(format),
                                                    .layers            = 1u,
                                                    .faces             = 1u,
                                                    .mip_levels        = 1u,
                                                    .format            = format,
                                                    .data              = std::move(output)

        };

        return image::Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveQOI(const image::Image& image, const std::filesystem::path& filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto result = saveQOI(image);

        if (!result) return core::makeUnexpected(result.error());

        auto& output = *result;

        auto stream = std::ofstream { filepath, std::ios::binary };
        core::write(stream, core::toConstByteSpan(output));

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    [[nodiscard]] auto saveQOI([[maybe_unused]] const image::Image& image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        return core::makeUnexpected(
            Error { .reason = Reason::Not_Implemented, .str_error = "save to file" });
    }
} // namespace stormkit::image::details
