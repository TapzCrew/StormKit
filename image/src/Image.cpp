/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - StormKit::core - ///////////
#include <storm/core/Numerics.hpp>
#include <storm/core/Ranges.hpp>
#include <storm/core/Strings.hpp>

/////////// - STL - ///////////
#include <fstream>
#include <streambuf>

/////////// - GSL - ///////////
#include <gsl/gsl_util>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    static constexpr auto KTX_HEADER = core::makeStaticByteArray(0xAB,
                                                                 0x4B,
                                                                 0x54,
                                                                 0x58,
                                                                 0x20,
                                                                 0x31,
                                                                 0x31,
                                                                 0xBB,
                                                                 0x0D,
                                                                 0x0A,
                                                                 0x1A,
                                                                 0x0A);

    static constexpr auto PNG_HEADER =
        core::makeStaticByteArray(0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A);

    static constexpr auto JPEG_HEADER = core::makeStaticByteArray(0xFF, 0xD8);

    Image::Codec filenameToCodec(const std::filesystem::path &filename) noexcept {
        STORMKIT_EXPECTS(std::filesystem::exists(filename));
        STORMKIT_EXPECTS(filename.has_extension());
        STORMKIT_EXPECTS(!std::filesystem::is_directory(filename));
        STORMKIT_EXPECTS(std::filesystem::is_regular_file(filename));

        const std::string ext = filename.extension().string();

        if (core::toLower(ext) == ".jpg" || core::toLower(ext) == ".jpeg")
            return Image::Codec::JPEG;
        else if (core::toLower(ext) == ".png")
            return Image::Codec::PNG;
        else if (core::toLower(ext) == ".tga" || core::toLower(ext) == ".targa")
            return Image::Codec::TARGA;
        else if (core::toLower(ext) == ".ppm")
            return Image::Codec::PPM;
        else if (core::toLower(ext) == ".hdr")
            return Image::Codec::HDR;
        else if (core::toLower(ext) == ".ktx")
            return Image::Codec::KTX;

        return Image::Codec::Unknown;
    }

    Image::Codec headerToCodec(core::ByteConstSpan data) noexcept {
        STORMKIT_EXPECTS(std::size(data) >= 12);

        if (std::memcmp(std::data(data), std::data(KTX_HEADER), std::size(KTX_HEADER)) == 0)
            return Image::Codec::KTX;
        else if (std::memcmp(std::data(data), std::data(PNG_HEADER), std::size(PNG_HEADER)) == 0)
            return Image::Codec::PNG;
        else if (std::memcmp(std::data(data), std::data(JPEG_HEADER), std::size(JPEG_HEADER)) == 0)
            return Image::Codec::JPEG;

        return Image::Codec::Unknown;
    }

    core::ByteArray map(core::ByteConstSpan bytes,
                        core::UInt8 source_count,
                        core::UInt8 destination_count) noexcept {
        STORMKIT_EXPECTS(source_count <= 4u && source_count > 0u && destination_count <= 4u &&
                         destination_count > 0u);

        static constexpr auto byte_1_min = std::numeric_limits<core::UInt8>::min();
        static constexpr auto byte_1_max = std::numeric_limits<core::UInt8>::max();
        static constexpr auto byte_2_min = std::numeric_limits<core::UInt16>::min();
        static constexpr auto byte_2_max = std::numeric_limits<core::UInt16>::max();
        static constexpr auto byte_4_min = std::numeric_limits<core::UInt32>::min();
        static constexpr auto byte_4_max = std::numeric_limits<core::UInt32>::max();

        auto data = core::ByteArray {};
        data.resize(std::size(bytes) * destination_count);

        if (source_count == 1u && destination_count == 2u) {
            const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt16>(input_it[i],
                                                       byte_1_min,
                                                       byte_1_max,
                                                       byte_2_min,
                                                       byte_2_max);
        } else if (source_count == 1u && destination_count == 4u) {
            const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt32>(input_it[i],
                                                       byte_1_min,
                                                       byte_1_max,
                                                       byte_4_min,
                                                       byte_4_max);
        } else if (source_count == 2u && destination_count == 1u) {
            const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt8>(input_it[i],
                                                      byte_2_min,
                                                      byte_2_max,
                                                      byte_1_min,
                                                      byte_1_max);
        } else if (source_count == 2u && destination_count == 4u) {
            const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt32>(input_it[i],
                                                       byte_2_min,
                                                       byte_2_max,
                                                       byte_4_min,
                                                       byte_4_max);
        } else if (source_count == 4u && destination_count == 1u) {
            const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt8>(input_it[i],
                                                      byte_4_min,
                                                      byte_4_max,
                                                      byte_1_min,
                                                      byte_1_max);
        } else if (source_count == 4u && destination_count == 2u) {
            const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
            auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

            for (auto i = 0u; i < std::size(bytes); ++i)
                output_it[i] = core::map<core::UInt16>(input_it[i],
                                                       byte_4_min,
                                                       byte_4_max,
                                                       byte_2_min,
                                                       byte_2_max);
        } else
            data = { core::ranges::begin(bytes), core::ranges::end(bytes) };

        return data;
    }
} // namespace storm::image

/////////////////////////////////////
/////////////////////////////////////
Image::Image() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Image::Image(const std::filesystem::path &filepath, Image::Codec codec) noexcept : Image {} {
    loadFromFile(filepath, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::Image(core::ByteConstSpan data, Image::Codec codec) noexcept : Image {} {
    loadFromMemory(data, codec);
}

/////////////////////////////////////
/////////////////////////////////////
Image::~Image() noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Image::Image(const Image &rhs) noexcept
    : m_extent { rhs.m_extent }, m_channel_count { rhs.m_channel_count },
      m_bytes_per_channel { rhs.m_bytes_per_channel }, m_layers { rhs.m_layers },
      m_faces { rhs.m_faces },
      m_mip_levels { rhs.m_mip_levels }, m_format { rhs.m_format }, m_data { rhs.m_data } {
}

////////////////////////////////////////
////////////////////////////////////////
Image &Image::operator=(const Image &rhs) noexcept {
    if (&rhs == this) return *this;

    m_channel_count     = rhs.m_channel_count;
    m_bytes_per_channel = rhs.m_bytes_per_channel;
    m_mip_levels        = rhs.m_mip_levels;
    m_format            = rhs.m_format;
    m_data              = rhs.m_data;
    m_extent            = rhs.m_extent;
    m_channel_count     = rhs.m_channel_count;
    m_bytes_per_channel = rhs.m_bytes_per_channel;
    m_mip_levels        = rhs.m_mip_levels;
    m_faces             = rhs.m_faces;
    m_layers            = rhs.m_layers;
    m_data              = rhs.m_data;
    m_format            = rhs.m_format;

    return *this;
};

////////////////////////////////////////
////////////////////////////////////////
Image::Image(Image &&) noexcept = default;

////////////////////////////////////////
////////////////////////////////////////
Image &Image::operator=(Image &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromFile(std::filesystem::path filepath, Image::Codec codec) noexcept {
    filepath = std::filesystem::canonical(filepath);

    STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
    STORMKIT_EXPECTS(!std::empty(filepath));

    if (!std::filesystem::exists(filepath)) {
        elog("Failed to open file\n    file: {}\n    reason: Incorrect path", filepath.string());

        return false;
    }

    STORMKIT_EXPECTS(std::filesystem::exists(filepath));

    const auto data = [&filepath]() {
        auto file = std::ifstream { filepath, std::ios::binary };

        file.seekg(0, std::ios::end);
        auto file_size = static_cast<std::size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        auto data = core::ByteArray { file_size };
        file.read(reinterpret_cast<char *>(std::data(data)), file_size);

        return data;
    }();

    if (codec == Image::Codec::Autodetect) codec = filenameToCodec(filepath);
    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = loadJPEG(data); result.has_value()) {
                elog("Failed to open JPEG file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = loadPNG(data); result.has_value()) {
                elog("Failed to open PNG file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = loadTARGA(data); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = loadPPM(data); result.has_value()) {
                elog("Failed to open PPM file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = loadHDR(data); result.has_value()) {
                elog("Failed to open HDR file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = loadKTX(data); result.has_value()) {
                elog("Failed to open KTX file\n    file: {}\n    reason: {}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }

        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::loadFromMemory(core::ByteConstSpan data, Image::Codec codec) noexcept {
    STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
    STORMKIT_EXPECTS(!std::empty(data));

    if (codec == Image::Codec::Autodetect) codec = headerToCodec(data);
    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = loadJPEG(data); result.has_value()) {
                elog("Failed to parse JPEG data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = loadPNG(data); result.has_value()) {
                elog("Failed to parse PNG data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = loadTARGA(data); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = loadPPM(data); result.has_value()) {
                elog("Failed to parse PPM data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = loadHDR(data); result.has_value()) {
                elog("Failed to parse HDR data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = loadKTX(data); result.has_value()) {
                elog("Failed to parse KTX data\n    reason: {}", result.value());
                return false;
            }

            return true;
        }
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
bool Image::saveToFile(std::filesystem::path filepath, Codec codec, CodecArgs args) const noexcept {
    filepath = std::filesystem::canonical(filepath.parent_path()) / filepath.filename();

    STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
    STORMKIT_EXPECTS(codec != Image::Codec::Autodetect);
    STORMKIT_EXPECTS(!std::empty(filepath));
    STORMKIT_EXPECTS(!std::empty(m_data));
    STORMKIT_EXPECTS(std::filesystem::exists(filepath.root_directory()));

    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = saveJPEG(filepath); result.has_value()) {
                elog("Failed to parse JPEG data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PNG: {
            if (auto result = savePNG(filepath); result.has_value()) {
                elog("Failed to save PNG file\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::TARGA: {
            if (auto result = saveTARGA(filepath); result.has_value()) {
                elog("Failed to open TARGA file\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::PPM: {
            if (auto result = savePPM(filepath, args); result.has_value()) {
                elog("Failed to parse PPM data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::HDR: {
            if (auto result = saveHDR(filepath); result.has_value()) {
                elog("Failed to parse HDR data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        case Image::Codec::KTX: {
            if (auto result = saveKTX(filepath); result.has_value()) {
                elog("Failed to parse KTX data\n    file: {}\n    reason{}",
                     filepath.string(),
                     result.value());
                return false;
            }

            return true;
        }
        default: return false;
    }

    return false;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<core::Byte> Image::saveToMemory(Codec codec, CodecArgs args) const noexcept
{
    STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
    STORMKIT_EXPECTS(codec != Image::Codec::Autodetect);
    STORMKIT_EXPECTS(!std::empty(m_data));

    auto output = core::ByteArray{};

    switch (codec) {
        case Image::Codec::JPEG: {
            if (auto result = saveJPEG(output); result.has_value()) {
                elog("Failed to parse JPEG data\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        case Image::Codec::PNG: {
            if (auto result = savePNG(output); result.has_value()) {
                elog("Failed to save PNG file\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        case Image::Codec::TARGA: {
            if (auto result = saveTARGA(output); result.has_value()) {
                elog("Failed to open TARGA file\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        case Image::Codec::PPM: {
            if (auto result = savePPM(output, args); result.has_value()) {
                elog("Failed to parse PPM data\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        case Image::Codec::HDR: {
            if (auto result = saveHDR(output); result.has_value()) {
                elog("Failed to parse HDR data\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        case Image::Codec::KTX: {
            if (auto result = saveKTX(output); result.has_value()) {
                elog("Failed to parse KTX data\n    reason{}",
                     result.value());
                return {};
            }

            return output;
        }
        default:
            return {};
    }

    return {};
}

/////////////////////////////////////
/////////////////////////////////////
void Image::create(core::Extentu extent, Format format) noexcept {
    STORMKIT_EXPECTS(extent.width > 0u && extent.height > 0u && extent.depth > 0u &&
                     format != Format::Undefined);
    m_data.clear();

    m_extent            = extent;
    m_channel_count     = getChannelCountFor(format);
    m_bytes_per_channel = getArraySizeByChannelFor(format);
    m_layers            = 1u;
    m_faces             = 1u;
    m_mip_levels        = 1u;
    m_format            = format;

    m_data.resize(m_extent.width * m_extent.height * m_extent.depth * m_layers * m_faces *
                  m_mip_levels * m_channel_count * m_bytes_per_channel);
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::toFormat(Format format) const noexcept {
    STORMKIT_EXPECTS(!std::empty(m_data));
    STORMKIT_EXPECTS(format != Format::Undefined);

    if (m_format == format) return *this;

    auto image                = Image {};
    image.m_extent            = m_extent;
    image.m_channel_count     = getChannelCountFor(format);
    image.m_bytes_per_channel = getArraySizeByChannelFor(format);
    image.m_mip_levels        = m_mip_levels;
    image.m_faces             = m_faces;
    image.m_layers            = m_layers;
    image.m_format            = format;

    /*const auto channel_delta =
        static_cast<core::UInt8>(std::max(0,
                                          static_cast<core::Int8>(image.m_channel_count) -
                                              static_cast<core::Int8>(m_channel_count)));*/
    const auto pixel_count = m_extent.width * m_extent.height * m_extent.depth;

    image.m_data.resize(pixel_count * image.m_channel_count * image.m_bytes_per_channel,
                        core::Byte { 1u });
    for (auto layer = 0u; layer < image.m_layers; ++layer) {
        for (auto face = 0u; face < image.m_faces; ++face) {
            for (auto level = 0u; level < image.m_layers; ++level) {
                for (auto i = 0u; i < pixel_count; ++i) {
                    const auto from_image = map(pixel(i, layer, face, level),
                                                m_bytes_per_channel,
                                                image.m_bytes_per_channel);
                    auto to_image         = image.pixel(i, layer, face, level);

                    core::ranges::copy_n(core::ranges::begin(from_image),
                                         image.m_channel_count,
                                         core::ranges::begin(to_image));
                }
            }
        }
    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::scale([[maybe_unused]] const core::Extentu &) const noexcept {
    return *this;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipX() const noexcept {
    auto image                = Image {};
    image.m_extent            = m_extent;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto layer = 0u; layer < m_layers; ++layer)
        for (auto face = 0u; face < m_faces; ++face)
            for (auto mip = 0u; mip < m_mip_levels; ++mip)
                for (auto x = 0u; x < m_extent.width; ++x) {
                    const auto inv_x = m_extent.width - x - 1u;
                    for (auto y = 0u; y < m_extent.height; ++y)
                        for (auto z = 0u; z < m_extent.depth; ++z) {
                            auto output =
                                image.pixel(core::Position3u { inv_x, y, z }, layer, face, mip);

                            core::ranges::copy(pixel(core::Position3u { x, y, z },
                                                     layer,
                                                     face,
                                                     mip),
                                               core::ranges::begin(output));
                        }
                }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipY() const noexcept {
    auto image                = Image {};
    image.m_extent            = m_extent;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto layer = 0u; layer < m_layers; ++layer)
        for (auto face = 0u; face < m_faces; ++face)
            for (auto mip = 0u; mip < m_mip_levels; ++mip)
                for (auto x = 0u; x < m_extent.width; ++x)
                    for (auto y = 0u; y < m_extent.height; ++y) {
                        const auto inv_y = m_extent.height - 1u - y;
                        for (auto z = 0u; z < m_extent.depth; ++z) {
                            auto output =
                                image.pixel(core::Position3u { x, inv_y, z }, layer, face, mip);
                            const auto data = pixel(core::Position3u { x, y, z }, layer, face, mip);

                            core::ranges::copy(data, core::ranges::begin(output));
                        }
                    }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::flipZ() const noexcept {
    auto image                = Image {};
    image.m_extent            = m_extent;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    for (auto layer = 0u; layer < m_layers; ++layer)
        for (auto face = 0u; face < m_faces; ++face)
            for (auto mip = 0u; mip < m_mip_levels; ++mip)
                for (auto x = 0u; x < m_extent.width; ++x)
                    for (auto y = 0u; y < m_extent.height; ++y)
                        for (auto z = 0u; z < m_extent.depth; ++z) {
                            const auto inv_z = m_extent.depth - 1u - z;

                            auto output =
                                image.pixel(core::Position3u { x, z, inv_z }, layer, face, mip);

                            core::ranges::copy(pixel(core::Position3u { x, y, z },
                                                     layer,
                                                     face,
                                                     mip),
                                               core::ranges::begin(output));
                        }

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate90() const noexcept {
    auto image                = Image {};
    image.m_extent.width      = m_extent.height;
    image.m_extent.height     = m_extent.width;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    // TODO implement

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate180() const noexcept {
    auto image                = Image {};
    image.m_extent            = m_extent;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
Image Image::rotate270() const noexcept {
    auto image                = Image {};
    image.m_extent.width      = m_extent.height;
    image.m_extent.height     = m_extent.width;
    image.m_channel_count     = m_channel_count;
    image.m_bytes_per_channel = m_bytes_per_channel;
    image.m_layers            = m_layers;
    image.m_faces             = m_faces;
    image.m_mip_levels        = m_mip_levels;
    image.m_format            = m_format;
    image.m_data.resize(std::size(m_data));

    return image;
}
