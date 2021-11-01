/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

#include <png.h>

using namespace storm;
using namespace storm::image;

struct PNGReadParam {
    core::ArraySize readed;
    core::ByteConstSpan &data;
};

struct PNGWriteParam {
    core::ByteArray &data;
};

static void png_read_func(png_struct *ps, png_byte *d, png_size_t length) noexcept {
    auto &param = *reinterpret_cast<PNGReadParam *>(png_get_io_ptr(ps));

    auto _d = core::toByteSpan(d, length);
    auto data = param.data.subspan(param.readed, length);

    core::ranges::copy(data, core::ranges::begin(_d));

    param.readed += length;
}

static void png_write_func(png_struct *ps, png_byte *d, png_size_t length) {
    auto &param = *reinterpret_cast<PNGWriteParam *>(png_get_io_ptr(ps));

    auto _d = core::toByteSpan(d, length);
    param.data.reserve(std::size(param.data) + length);

    core::ranges::copy(_d, std::back_inserter(param.data));
}

std::optional<std::string> Image::loadPNG(core::ByteConstSpan data) noexcept {
    auto image_data = core::ByteArray {};
    auto format     = Format {};
    auto extent     = core::Extentu {};

    auto read_param = PNGReadParam { 8u, data };

    auto sig = reinterpret_cast<png_const_bytep>(std::data(data));
    if (!png_check_sig(sig, 8u)) return "[libpng] Failed to validate PNG signature";

    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return "[libpng] Failed to init (png_create_read_struct)";

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        return "[libpng] Failed to init (png_create_info_struct)";
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    }

    png_set_read_fn(png_ptr, &read_param, png_read_func);
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
    image_data.resize(extent.height * row_bytes);

    auto row_pointers = std::vector<core::Byte *> { extent.height, nullptr };

    auto buff_pos = std::data(image_data);

    for (auto i = 0u; i < extent.height; ++i) row_pointers[i] = &buff_pos[row_bytes * i];

    png_read_image(png_ptr, reinterpret_cast<png_bytepp>(std::data(row_pointers)));
    png_read_end(png_ptr, info_ptr);

    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);

    m_extent            = std::move(extent);
    m_channel_count     = getChannelCountFor(format);
    m_bytes_per_channel = getArraySizeByChannelFor(format);
    m_mip_levels        = 1u;
    m_faces             = 1u;
    m_layers            = 1u;
    m_data              = std::move(image_data);
    m_format            = format;

    return std::nullopt;
}

std::optional<std::string> Image::savePNG(const std::filesystem::path &filepath) const noexcept {
    auto _filename = filepath;

    auto output = core::ByteArray{};

    if(auto result = savePNG(output))
        return result;

    auto stream = std::ofstream{filepath, std::ios::binary};
    stream.write(reinterpret_cast<const char *>(std::data(output)), std::size(output));

    return std::nullopt;
}

std::optional<std::string> Image::savePNG(core::ByteArray &output) const noexcept {
    auto write_param = PNGWriteParam { output };

    auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return "[libpng] Failed to init (png_create_write_struct)";

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return "[libpng] Failed to init (png_create_info_struct)";
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_info_struct(png_ptr, &info_ptr);
        png_destroy_write_struct(&png_ptr, nullptr);
        return "[libpng] Error during png creation";
    }

    png_set_write_fn(png_ptr, &write_param, png_write_func, nullptr);

    png_set_IHDR(png_ptr,
                 info_ptr,
                 m_extent.width,
                 m_extent.height,
                 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    auto rows = std::vector<core::Byte*>{m_extent.height, nullptr};
    for(auto i = 0u; i < m_extent.height; ++i)
        rows[i] = const_cast<core::Byte*>(&m_data[i * m_extent.width * m_channel_count * m_bytes_per_channel]); // TODO Fix this shit

    png_set_rows(png_ptr, info_ptr, reinterpret_cast<png_bytepp>(std::data(rows)));
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_end(png_ptr, info_ptr);

    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_write_struct(&png_ptr, nullptr);

    return std::nullopt;
}
