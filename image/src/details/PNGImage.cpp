// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <ranges>

#include "PNGImage.mpp"

#include <csetjmp>

#include <png.h>

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;
    using Format     = image::Image::Format;

    namespace png {
        struct ReadParam {
            core::USize readed;
            core::ByteConstSpan& data;
        };

        struct WriteParam {
            core::ByteArray& data;
        };

        /////////////////////////////////////
        /////////////////////////////////////
        static auto read_func(png_struct *ps, png_byte *d, png_size_t length) noexcept -> void {
            auto& param = *reinterpret_cast<ReadParam *>(png_get_io_ptr(ps));

            auto _d   = core::toByteSpan(d, length);
            auto data = param.data.subspan(param.readed, length);

            std::ranges::copy(data, std::ranges::begin(_d));

            param.readed += length;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        static auto write_func(png_struct *ps, png_byte *d, png_size_t length) -> void {
            auto& param = *reinterpret_cast<WriteParam *>(png_get_io_ptr(ps));

            auto _d = core::toByteSpan(d, length);
            param.data.reserve(std::size(param.data) + length);

            std::ranges::copy(_d, std::back_inserter(param.data));
        }
    } // namespace png

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadPNG(core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        auto image_memory = core::ByteArray {};
        auto format       = Format {};
        auto extent       = core::ExtentU {};

        auto read_param = png::ReadParam { 8u, data };

        auto sig = reinterpret_cast<png_const_bytep>(std::data(data));
        if (!png_check_sig(sig, 8u))
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Failed to validate PNG signature" });

        auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr)
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Failed to init (png_create_read_struct)" });

        auto info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Failed to init (png_create_info_struct)" });
        }

        png_set_read_fn(png_ptr, &read_param, png::read_func);
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        auto bit_depth  = 0;
        auto color_type = 0;

        png_get_IHDR(png_ptr,
                     info_ptr,
                     &extent.width,
                     &extent.height,
                     &bit_depth,
                     &color_type,
                     nullptr,
                     nullptr,
                     nullptr);

        if (color_type == PNG_COLOR_TYPE_GRAY) png_set_expand_gray_1_2_4_to_8(png_ptr);
        else if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_palette_to_rgb(png_ptr);
            png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
        }
        if (bit_depth < 8) png_set_packing(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

        png_get_IHDR(png_ptr,
                     info_ptr,
                     &extent.width,
                     &extent.height,
                     &bit_depth,
                     &color_type,
                     nullptr,
                     nullptr,
                     nullptr);

        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY: {
                if (bit_depth == 8) format = Format::R8_UNorm;
                else if (bit_depth == 16)
                    format = Format::R16_UNorm;

                break;
            }
            case PNG_COLOR_TYPE_GRAY_ALPHA: {
                if (bit_depth == 8) format = Format::RG8_UNorm;
                else if (bit_depth == 16)
                    format = Format::RG16_UNorm;

                break;
            }
            case PNG_COLOR_TYPE_RGB: {
                if (bit_depth == 8) format = Format::RGB8_UNorm;
                else if (bit_depth == 16)
                    format = Format::RGB16_UNorm;

                break;
            }
            case PNG_COLOR_TYPE_RGB_ALPHA: {
                if (bit_depth == 8) format = Format::RGBA8_UNorm;
                else if (bit_depth == 16)
                    format = Format::RGBA16_UNorm;

                break;
            }
            case PNG_COLOR_TYPE_PALETTE: {
                if (bit_depth == 8) format = Format::RGBA8_UNorm;
                else if (bit_depth == 16)
                    format = Format::RGBA16_UNorm;

                break;
            }

            default: break;
        }

        png_read_update_info(png_ptr, info_ptr);

        const auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);
        image_memory.resize(extent.height * row_bytes);

        auto row_pointers = std::vector<core::Byte *> { extent.height, nullptr };

        auto buff_pos = std::data(image_memory);

        for (auto i : core::range(extent.height)) row_pointers[i] = &buff_pos[row_bytes * i];

        png_read_image(png_ptr, reinterpret_cast<png_bytepp>(std::data(row_pointers)));
        png_read_end(png_ptr, info_ptr);

        png_destroy_info_struct(png_ptr, &info_ptr);
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);

        auto image_data = image::Image::ImageData { .extent            = std::move(extent),
                                                    .channel_count     = getChannelCountFor(format),
                                                    .bytes_per_channel = getSizeof(format),
                                                    .layers            = 1u,
                                                    .faces             = 1u,
                                                    .mip_levels        = 1u,
                                                    .format            = format,
                                                    .data              = std::move(image_memory) };

        return Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto savePNG(const image::Image& image, const std::filesystem::path& filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto result = savePNG(image);

        if (!result) return core::makeUnexpected(result.error());

        auto &output = *result;

        auto stream = std::ofstream { filepath, std::ios::binary };
        stream.write(reinterpret_cast<const char *>(std::data(output)), std::size(output));

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto savePNG(const image::Image& image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        auto output = core::ByteArray {};

        auto write_param = png::WriteParam { output };

        auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr)
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Failed to init (png_create_write_struct)" });

        auto info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            png_destroy_write_struct(&png_ptr, nullptr);
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Failed to init (png_create_info_struct)" });
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_info_struct(png_ptr, &info_ptr);
            png_destroy_write_struct(&png_ptr, nullptr);
            return core::makeUnexpected(
                Error { .reason    = Reason::Failed_To_Parse,
                        .str_error = "[libpng] Unkown error during png creation" });
        }

        png_set_write_fn(png_ptr, &write_param, png::write_func, nullptr);

        const auto& data = image.imageData();

        png_set_IHDR(png_ptr,
                     info_ptr,
                     data.extent.width,
                     data.extent.height,
                     8,
                     PNG_COLOR_TYPE_RGB_ALPHA,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png_ptr, info_ptr);

        auto rows = std::vector<core::Byte *> { data.extent.height, nullptr };
        for (auto i : core::range(data.extent.height))
            rows[i] =
                const_cast<core::Byte *>(&data.data[i * data.extent.width * data.channel_count *
                                                    data.bytes_per_channel]); // TODO Fix this shit

        png_set_rows(png_ptr, info_ptr, reinterpret_cast<png_bytepp>(std::data(rows)));
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
        png_write_end(png_ptr, info_ptr);

        png_destroy_info_struct(png_ptr, &info_ptr);
        png_destroy_write_struct(&png_ptr, nullptr);

        return output;
        ;
    }
} // namespace stormkit::image::details
