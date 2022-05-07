// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.image.details.jpgimage;

// clang-format off
/////////// - STL - ///////////
import <ranges>;
// clang-format on
#else
    /////////// - STL - ///////////
    #include <ranges>

    /////////// - StormKit::image - ///////////
    #include "JPEGImage.mpp"
#endif

/////////// - STL - ///////////
#include <csetjmp>

/////////// - libJPEG - ///////////
#include <jerror.h>
#include <jpeglib.h>

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;
    using Format     = image::Image::Format;

    namespace jpg {
        struct ErrorData {
            std::jmp_buf setjmp_buffer;
            std::string msg;
        };

        /////////////////////////////////////
        /////////////////////////////////////
        static void error_callback(jpeg_common_struct *st) noexcept {
            STORMKIT_EXPECTS(st != nullptr);

            auto error_data = reinterpret_cast<ErrorData *>(st->client_data);

            auto message = std::string {};
            message.resize(JMSG_STR_PARM_MAX);
            (*st->err->format_message)(st, std::data(message));

            error_data->msg = message;

            std::longjmp(error_data->setjmp_buffer, 1);
        }
    } // namespace jpg

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadJPG(core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        auto image_memory = core::ByteArray {};
        auto format       = Format {};
        auto extent       = core::ExtentU {};

        auto info      = jpeg_decompress_struct {};
        auto error_mgr = jpeg_error_mgr {};

        auto error_data = jpg::ErrorData {};

        info.err             = jpeg_std_error(&error_mgr);
        info.client_data     = &error_data;
        error_mgr.error_exit = jpg::error_callback;

        jpeg_create_decompress(&info);
        if (setjmp(error_data.setjmp_buffer)) {
            return core::Unexpected { Error { .reason    = Reason::Failed_To_Parse,
                                              .str_error = error_data.msg } };
        }

        jpeg_mem_src(&info,
                     reinterpret_cast<const unsigned char *>(std::data(data)),
                     std::size(data));
        jpeg_read_header(&info, TRUE);

        jpeg_start_decompress(&info);
        extent.width  = info.output_width;
        extent.height = info.output_height;
        extent.depth  = 1;
        if (info.output_components == 1) format = Format::R8_UNorm;
        if (info.output_components == 2) format = Format::RG8_UNorm;
        if (info.output_components == 3) format = Format::RGB8_UNorm;

        image_memory.resize(extent.width * extent.height * extent.depth *
                            info.out_color_components);

        auto row_ptr = std::array<core::Byte *, 1> { nullptr };
        while (info.output_scanline < info.output_height) {
            const auto index = extent.width * info.output_components * info.output_scanline;
            row_ptr[0]       = std::data(image_memory) + index;
            jpeg_read_scanlines(&info,
                                reinterpret_cast<JSAMPARRAY>(std::data(row_ptr)),
                                core::as<JDIMENSION>(std::size(row_ptr)));
        }

        jpeg_finish_decompress(&info);
        jpeg_destroy_decompress(&info);

        if (setjmp(error_data.setjmp_buffer)) {
            jpeg_destroy_decompress(&info);
            return core::Unexpected { Error { .reason    = Reason::Failed_To_Parse,
                                              .str_error = error_data.msg } };
        }

        auto image_data = image::Image::ImageData {};

        image_data.extent            = extent;
        image_data.channel_count     = getChannelCountFor(format);
        image_data.bytes_per_channel = 1u;
        image_data.mip_levels        = 1u;
        image_data.faces             = 1u;
        image_data.layers            = 1u;
        image_data.data              = std::move(image_memory);
        image_data.format            = format;

        return Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveJPG(const image::Image &image, const std::filesystem::path &filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto _filename = filepath;

        auto image_rgb = image.toFormat(Format::RGB8_UNorm);

        auto info      = jpeg_compress_struct {};
        auto error_mgr = jpeg_error_mgr {};

        auto error_data = jpg::ErrorData {};

        info.err             = jpeg_std_error(&error_mgr);
        info.client_data     = &error_data;
        error_mgr.error_exit = jpg::error_callback;

        for (auto i = 0u; i < image_rgb.mipLevels(); ++i) {
            if (i >= 1u) _filename += core::format("_mip{}", i);

            auto file = std::fopen(_filename.string().c_str(), "wb");
            if (file == nullptr) {
                auto error = std::string { 95 };
                error      = std::strerror(errno);
                error.shrink_to_fit();

                return core::Unexpected { Error { .reason    = Reason::Failed_To_Save,
                                                  .str_error = error_data.msg } };
            }

            auto data          = image_rgb.data(0, 0, 0);
            const auto &extent = image_rgb.extent(0);

            jpeg_create_compress(&info);
            jpeg_stdio_dest(&info, file);

            info.image_width      = extent.width;
            info.image_height     = extent.height;
            info.input_components = getChannelCountFor(Format::RGB8_UNorm);
            info.in_color_space   = JCS_RGB;
            jpeg_set_defaults(&info);
            jpeg_set_quality(&info, 75, TRUE);

            jpeg_start_compress(&info, TRUE);

            auto row_ptr = std::array<core::Byte *, 1> { nullptr };
            while (info.next_scanline < info.image_height) {
                const auto index = info.next_scanline * 3u * info.image_width;
                row_ptr[0]       = std::data(data) + index;

                jpeg_write_scanlines(&info, reinterpret_cast<JSAMPARRAY>(std::data(row_ptr)), 1);
            }

            jpeg_finish_compress(&info);
            jpeg_destroy_compress(&info);

            std::fflush(file);
            std::fclose(file);
        }

        if (setjmp(error_data.setjmp_buffer)) {
            jpeg_destroy_compress(&info);
            return core::Unexpected { Error { .reason    = Reason::Failed_To_Save,
                                              .str_error = error_data.msg } };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveJPG(const image::Image &image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        using uchar_ptr = unsigned char *;

        auto output_ptr = uchar_ptr { nullptr };

        auto image_rgb = image.toFormat(Format::RGB8_UNorm);

        auto info      = jpeg_compress_struct {};
        auto error_mgr = jpeg_error_mgr {};

        auto error_data = jpg::ErrorData {};

        info.err             = jpeg_std_error(&error_mgr);
        info.client_data     = &error_data;
        error_mgr.error_exit = jpg::error_callback;

        auto data          = image_rgb.data(0, 0, 0);
        const auto &extent = image_rgb.extent(0);

        jpeg_create_compress(&info);

        auto out_size = std::size_t { 0 };
        jpeg_mem_dest(&info, &output_ptr, &out_size);

        info.image_width      = extent.width;
        info.image_height     = extent.height;
        info.input_components = getChannelCountFor(Format::RGB8_UNorm);
        info.in_color_space   = JCS_RGB;
        jpeg_set_defaults(&info);
        jpeg_set_quality(&info, 75, TRUE);

        jpeg_start_compress(&info, TRUE);

        auto row_ptr = std::array<core::Byte *, 1> { nullptr };
        while (info.next_scanline < info.image_height) {
            const auto index = info.next_scanline * 3u * info.image_width;
            row_ptr[0]       = std::data(data) + index;

            jpeg_write_scanlines(&info, reinterpret_cast<JSAMPARRAY>(std::data(row_ptr)), 1);
        }

        jpeg_finish_compress(&info);
        jpeg_destroy_compress(&info);

        if (setjmp(error_data.setjmp_buffer)) {
            jpeg_destroy_compress(&info);
            return core::Unexpected { Error { .reason    = Reason::Failed_To_Save,
                                              .str_error = error_data.msg } };
        }

        auto output = core::ByteArray {};
        output.reserve((out_size));

        std::ranges::copy(core::toConstByteSpan(output_ptr, out_size), std::back_inserter(output));
        if (output_ptr != nullptr) std::free(output_ptr);

        return output;
    }
} // namespace stormkit::image::details
