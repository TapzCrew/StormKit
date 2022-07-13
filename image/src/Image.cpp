// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <string>

#include <stormkit/core/Coroutines.mpp>
#include <stormkit/core/Format.mpp>
#include <stormkit/core/Numerics.mpp>
#include <stormkit/core/Strings.mpp>
#include <stormkit/core/Types.mpp>

#include <stormkit/image/Image.mpp>

#include "details/HDRImage.mpp"
#include "details/JPEGImage.mpp"
#include "details/KTXImage.mpp"
#include "details/PNGImage.mpp"
#include "details/PPMImage.mpp"
#include "details/QOIImage.mpp"
#include "details/TARGAImage.mpp"

namespace stormkit::image {
    namespace details {
        using namespace stormkit::core::literals;
        constexpr auto KTX_HEADER = core::makeStaticByteArray(0xAB_b,
                                                              0x4B_b,
                                                              0x54_b,
                                                              0x58_b,
                                                              0x20_b,
                                                              0x31_b,
                                                              0x31_b,
                                                              0xBB_b,
                                                              0x0D_b,
                                                              0x0A_b,
                                                              0x1A_b,
                                                              0x0A_b);

        constexpr auto PNG_HEADER = core::makeStaticByteArray(0x89_b,
                                                              0x50_b,
                                                              0x4E_b,
                                                              0x47_b,
                                                              0x0D_b,
                                                              0x0A_b,
                                                              0x1A_b,
                                                              0x0A_b);

        constexpr auto QOI_HEADER = core::makeStaticByteArray(0x71_b, 0x6f_b, 0x69_b, 0x66_b);

        constexpr auto JPEG_HEADER = core::makeStaticByteArray(0xFF_b, 0xD8_b);

        auto filenameToCodec(const std::filesystem::path& filename) noexcept -> Image::Codec {
            STORMKIT_EXPECTS(std::filesystem::exists(filename));
            STORMKIT_EXPECTS(filename.has_extension());
            STORMKIT_EXPECTS(!std::filesystem::is_directory(filename));
            STORMKIT_EXPECTS(std::filesystem::is_regular_file(filename));

            const auto ext = filename.extension().string();

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
            else if (core::toLower(ext) == ".qoi")
                return Image::Codec::QOI;

            return Image::Codec::Unknown;
        }

        auto headerToCodec(core::ByteConstSpan data) noexcept -> Image::Codec {
            STORMKIT_EXPECTS(std::size(data) >= 12);

            if (std::memcmp(std::data(data), std::data(KTX_HEADER), std::size(KTX_HEADER)) == 0)
                return Image::Codec::KTX;
            else if (std::memcmp(std::data(data), std::data(PNG_HEADER), std::size(PNG_HEADER)) ==
                     0)
                return Image::Codec::PNG;
            else if (std::memcmp(std::data(data), std::data(JPEG_HEADER), std::size(JPEG_HEADER)) ==
                     0)
                return Image::Codec::JPEG;
            else if (std::memcmp(std::data(data), std::data(QOI_HEADER), std::size(QOI_HEADER)) ==
                     0)
                return Image::Codec::QOI;

            return Image::Codec::Unknown;
        }

        auto map(core::ByteConstSpan bytes,
                 core::UInt32 source_count,
                 core::UInt32 destination_count) noexcept -> core::ByteArray {
            STORMKIT_EXPECTS(source_count <= 4u && source_count > 0u && destination_count <= 4u &&
                             destination_count > 0u);

            constexpr auto BYTE_1_MIN = std::numeric_limits<core::UInt8>::min();
            constexpr auto BYTE_1_MAX = std::numeric_limits<core::UInt8>::max();
            constexpr auto BYTE_2_MIN = std::numeric_limits<core::UInt16>::min();
            constexpr auto BYTE_2_MAX = std::numeric_limits<core::UInt16>::max();
            constexpr auto BYTE_4_MIN = std::numeric_limits<core::UInt32>::min();
            constexpr auto BYTE_4_MAX = std::numeric_limits<core::UInt32>::max();

            auto data = core::ByteArray {};
            data.resize(std::size(bytes) * destination_count);

            if (source_count == 1u && destination_count == 2u) {
                const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt16>(input_it[i],
                                                           BYTE_1_MIN,
                                                           BYTE_1_MAX,
                                                           BYTE_2_MIN,
                                                           BYTE_2_MAX);
            } else if (source_count == 1u && destination_count == 4u) {
                const auto input_it = reinterpret_cast<const core::UInt8 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt32>(input_it[i],
                                                           BYTE_1_MIN,
                                                           BYTE_1_MAX,
                                                           BYTE_4_MIN,
                                                           BYTE_4_MAX);
            } else if (source_count == 2u && destination_count == 1u) {
                const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt8>(input_it[i],
                                                          BYTE_2_MIN,
                                                          BYTE_2_MAX,
                                                          BYTE_1_MIN,
                                                          BYTE_1_MAX);
            } else if (source_count == 2u && destination_count == 4u) {
                const auto input_it = reinterpret_cast<const core::UInt16 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt32 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt32>(input_it[i],
                                                           BYTE_2_MIN,
                                                           BYTE_2_MAX,
                                                           BYTE_4_MIN,
                                                           BYTE_4_MAX);
            } else if (source_count == 4u && destination_count == 1u) {
                const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt8 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt8>(input_it[i],
                                                          BYTE_4_MIN,
                                                          BYTE_4_MAX,
                                                          BYTE_1_MIN,
                                                          BYTE_1_MAX);
            } else if (source_count == 4u && destination_count == 2u) {
                const auto input_it = reinterpret_cast<const core::UInt32 *>(std::data(data));
                auto output_it      = reinterpret_cast<core::UInt16 *>(std::data(data));

                for (auto i : core::range(std::size(bytes)))
                    output_it[i] = core::map<core::UInt16>(input_it[i],
                                                           BYTE_4_MIN,
                                                           BYTE_4_MAX,
                                                           BYTE_2_MIN,
                                                           BYTE_2_MAX);
            } else
                data = { std::ranges::begin(bytes), std::ranges::end(bytes) };

            return data;
        }
    } // namespace details

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(ImageData&& data) noexcept : Image {} {
        m_data = std::move(data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const core::ExtentU& extent, Format format) noexcept : Image {} {
        create(extent, format);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(const std::filesystem::path& filepath, Image::Codec codec) noexcept : Image {} {
        [[maybe_unused]] const auto _ = loadFromFile(filepath, codec);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Image::Image(core::ByteConstSpan data, Image::Codec codec) noexcept : Image {} {
        [[maybe_unused]] const auto _ = loadFromMemory(data, codec);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    Image::Image(const Image& rhs) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Image::operator=(const Image& rhs) noexcept -> Image& = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    Image::Image(Image&&) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto Image::operator=(Image&&) noexcept -> Image& = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    Image::~Image() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromFile(std::filesystem::path filepath, Image::Codec codec) noexcept
        -> core::Expected<void, Error> {
        filepath = std::filesystem::canonical(filepath);

        STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
        STORMKIT_EXPECTS(!std::empty(filepath));

        if (!std::filesystem::exists(filepath)) {
            return core::makeUnexpected(
                Error { .reason    = Error::Reason::File_Not_Found,
                        .str_error = core::format("Failed to open file {}\n    > Incorrect path",
                                                  filepath.string()) });
        }

        const auto data = [&filepath]() {
            auto file = std::ifstream { filepath, std::ios::binary };

            file.seekg(0, std::ios::end);
            auto file_size = core::as<std::size_t>(std::streamoff { file.tellg() });
            file.seekg(0, std::ios::beg);

            auto data = core::ByteArray { file_size };
            file.read(reinterpret_cast<char *>(std::data(data)), file_size);

            return data;
        }();

        if (codec == Image::Codec::Autodetect) codec = details::filenameToCodec(filepath);
        switch (codec) {
            case Image::Codec::JPEG: {
                auto result = details::loadJPG(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::PNG: {
                auto result = details::loadPNG(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                } // namespace stormkit::image

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::TARGA: {
                auto result = details::loadTGA(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::PPM: {
                auto result = details::loadPPM(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::HDR: {
                auto result = details::loadHDR(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::KTX: {
                auto result = details::loadKTX(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::QOI: {
                auto result = details::loadQOI(data);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            default: break;
        }

        return core::makeUnexpected(
            Error { .reason    = Error::Reason::Invalid_Format,
                    .str_error = core::format("Failed to save image from {}\n    > Invalid format",
                                              filepath.string()) });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::loadFromMemory(core::ByteConstSpan data, Image::Codec codec) noexcept
        -> core::Expected<void, Error> {
        STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
        STORMKIT_EXPECTS(!std::empty(data));

        if (codec == Image::Codec::Autodetect) codec = details::headerToCodec(data);
        switch (codec) {
            case Image::Codec::JPEG: {
                auto result = details::loadJPG(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load JPEG image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::PNG: {
                auto result = details::loadPNG(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load PNG image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::TARGA: {
                auto result = details::loadTGA(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load TARGA image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::PPM: {
                auto result = details::loadPPM(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load PPM image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::HDR: {
                auto result = details::loadHDR(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load HDR image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::KTX: {
                auto result = details::loadKTX(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            case Image::Codec::QOI: {
                auto result = details::loadQOI(data);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load QOI image from data\n    > {}",
                                                  result.error().str_error) });
                }

                *this = std::move(*result);

                return {};
            }
            default: break;
        }

        return core::makeUnexpected(
            Error { .reason    = Error::Reason::Invalid_Format,
                    .str_error = "Failed to load image\n    > Invalid format" });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::saveToFile(std::filesystem::path filepath,
                           Codec codec,
                           CodecArgs args) const noexcept -> core::Expected<void, Error> {
        filepath = std::filesystem::canonical(filepath.parent_path()) / filepath.filename();

        STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
        STORMKIT_EXPECTS(codec != Image::Codec::Autodetect);
        STORMKIT_EXPECTS(!std::empty(filepath));
        STORMKIT_EXPECTS(!std::empty(m_data.data));
        STORMKIT_EXPECTS(std::filesystem::exists(filepath.root_directory()));

        switch (codec) {
            case Image::Codec::JPEG: {
                auto result = details::saveJPG(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to save to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::PNG: {
                auto result = details::savePNG(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::TARGA: {
                auto result = details::saveTGA(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::PPM: {
                auto result = details::savePPM(*this, args, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::HDR: {
                auto result = details::saveHDR(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::KTX: {
                auto result = details::saveKTX(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }
            case Image::Codec::QOI: {
                auto result = details::saveQOI(*this, filepath);
                if (!result) {
                    return core::makeUnexpected(
                        Error { .reason    = result.error().reason,
                                .str_error = core::format("Failed to load to file {}\n    > {}",
                                                          filepath.string(),
                                                          result.error().str_error) });
                }
                return {};
            }

            default: break;
        }

        return core::makeUnexpected(
            Error { .reason    = Error::Reason::Invalid_Format,
                    .str_error = core::format("Failed to save image to {}\n    > Invalid format",
                                              filepath.string()) });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::saveToMemory(Codec codec, CodecArgs args) const noexcept
        -> core::Expected<core::ByteArray, Error> {
        STORMKIT_EXPECTS(codec != Image::Codec::Unknown);
        STORMKIT_EXPECTS(codec != Image::Codec::Autodetect);
        STORMKIT_EXPECTS(!std::empty(m_data.data));

        auto output = core::ByteArray {};

        switch (codec) {
            case Image::Codec::JPEG: {
                auto result = details::saveJPG(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::PNG: {
                auto result = details::savePNG(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::TARGA: {
                auto result = details::saveTGA(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::PPM: {
                auto result = details::savePPM(*this, args);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::HDR: {
                auto result = details::saveHDR(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::KTX: {
                auto result = details::saveKTX(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load KTX image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }
            case Image::Codec::QOI: {
                auto result = details::saveQOI(*this);
                if (!result) {
                    return core::makeUnexpected(Error {
                        .reason    = result.error().reason,
                        .str_error = core::format("Failed to load QOI image from data\n    > {}",
                                                  result.error().str_error) });
                }

                return *result;
            }

            default: break;
        }

        return core::makeUnexpected(
            Error { .reason    = Error::Reason::Invalid_Format,
                    .str_error = "Failed to save image\n    > Invalid format" });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::create(core::ExtentU extent, Format format) noexcept -> void {
        STORMKIT_EXPECTS(extent.width > 0u && extent.height > 0u && extent.depth > 0u &&
                         format != Format::Undefined);
        m_data.data.clear();

        m_data.extent            = extent;
        m_data.channel_count     = getChannelCountFor(format);
        m_data.bytes_per_channel = getSizeof(format);
        m_data.layers            = 1u;
        m_data.faces             = 1u;
        m_data.mip_levels        = 1u;
        m_data.format            = format;

        m_data.data.resize(m_data.extent.width * m_data.extent.height * m_data.extent.depth *
                           m_data.layers * m_data.faces * m_data.mip_levels * m_data.channel_count *
                           m_data.bytes_per_channel);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::toFormat(Format format) const noexcept -> Image {
        STORMKIT_EXPECTS(!std::empty(m_data.data));
        STORMKIT_EXPECTS(format != Format::Undefined);

        if (m_data.format == format) return *this;

        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = getChannelCountFor(format),
                                      .bytes_per_channel = getSizeof(format),
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = format };

        /*const auto channel_delta =
            static_cast<core::UInt8>(std::max(0,
                                              static_cast<core::Int8>(image_data.channel_count) -
                                                  static_cast<core::Int8>(m_data.channel_count)));*/
        const auto pixel_count = m_data.extent.width * m_data.extent.height * m_data.extent.depth;

        image_data.data.resize(pixel_count * image_data.channel_count *
                                   image_data.bytes_per_channel,
                               core::Byte { 255u });

        auto image = Image { std::move(image_data) };

        for (auto [layer, face, level, i] : core::generateIndicesAs<core::UInt32>(image.layers(),
                                                                                  image.faces(),
                                                                                  image.layers(),
                                                                                  pixel_count)) {
            const auto from_image =
                details::map(pixel(core::as<core::USize>(i), layer, face, level),
                             m_data.bytes_per_channel,
                             image.bytesPerChannel());
            auto to_image = image.pixel(core::as<core::USize>(i), layer, face, level);

            std::ranges::copy_n(std::ranges::begin(from_image),
                                std::min(m_data.channel_count, image.channelCount()),
                                std::ranges::begin(to_image));
        }

        return image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::scale([[maybe_unused]] const core::ExtentU&) const noexcept -> Image {
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::flipX() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };

        image_data.data.resize(std::size(m_data.data));

        auto image = Image { std::move(image_data) };

        for (auto [layer, face, mip, x, y, z] :
             core::generateIndicesAs<core::UInt32>(m_data.layers,
                                                   m_data.faces,
                                                   m_data.mip_levels,
                                                   m_data.extent.width,
                                                   m_data.extent.height,
                                                   m_data.extent.depth)) {
            const auto inv_x = m_data.extent.width - x - 1u;
            auto output      = image.pixel({ inv_x, y, z }, layer, face, mip);
            // const auto data  = pixel({ x, y, z }, layer, face, mip);

            std::ranges::copy(pixel({ x, y, z }, layer, face, mip), std::ranges::begin(output));
        }

        return image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::flipY() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };

        image_data.data.resize(std::size(m_data.data));

        auto image = Image { std::move(image_data) };

        for (auto [layer, face, mip, x, y, z] :
             core::generateIndicesAs<core::UInt32>(m_data.layers,
                                                   m_data.faces,
                                                   m_data.mip_levels,
                                                   m_data.extent.width,
                                                   m_data.extent.height,
                                                   m_data.extent.depth)) {
            const auto inv_y = m_data.extent.height - 1u - y;
            auto output      = image.pixel({ x, inv_y, z }, layer, face, mip);

            std::ranges::copy(pixel({ x, y, z }, layer, face, mip), std::ranges::begin(output));
        }

        return image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::flipZ() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };
        image_data.data.resize(std::size(m_data.data));

        auto image = Image { std::move(image_data) };

        for (auto [layer, face, mip, x, y, z] :
             core::generateIndicesAs<core::UInt32>(m_data.layers,
                                                   m_data.faces,
                                                   m_data.mip_levels,
                                                   m_data.extent.width,
                                                   m_data.extent.height,
                                                   m_data.extent.depth)) {
            const auto inv_z = m_data.extent.depth - 1u - z;
            auto output      = image.pixel({ x, z, inv_z }, layer, face, mip);

            std::ranges::copy(pixel({ x, y, z }, layer, face, mip), std::ranges::begin(output));
        }

        return image;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::rotate90() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };

        image_data.data.resize(std::size(m_data.data));

        // TODO implement

        return Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::rotate180() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };

        image_data.data.resize(std::size(m_data.data));

        return Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Image::rotate270() const noexcept -> Image {
        auto image_data = ImageData { .extent            = m_data.extent,
                                      .channel_count     = m_data.channel_count,
                                      .bytes_per_channel = m_data.bytes_per_channel,
                                      .layers            = m_data.layers,
                                      .faces             = m_data.faces,
                                      .mip_levels        = m_data.mip_levels,
                                      .format            = m_data.format };

        image_data.data.resize(std::size(m_data.data));

        return Image { std::move(image_data) };
    }

} // namespace stormkit::image
