// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <ranges>

#include <gli/gli.hpp>

#include "KTXImage.mpp"

#include <csetjmp>
#include <cstring>

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;
    using Format     = image::Image::Format;

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto toStormFormat(gli::format format) noexcept {
        switch (format) {
            case gli::FORMAT_R8_SNORM_PACK8: return Format::R8_SNorm;
            case gli::FORMAT_R8_UNORM_PACK8: return Format::R8_UNorm;
            case gli::FORMAT_R16_SNORM_PACK16: return Format::R16_SNorm;
            case gli::FORMAT_R16_UNORM_PACK16: return Format::R16_UNorm;
            case gli::FORMAT_R8_SINT_PACK8: return Format::R8I;
            case gli::FORMAT_R8_UINT_PACK8: return Format::R8U;
            case gli::FORMAT_R16_SINT_PACK16: return Format::R16I;
            case gli::FORMAT_R16_UINT_PACK16: return Format::R16U;
            case gli::FORMAT_R32_SINT_PACK32: return Format::R32I;
            case gli::FORMAT_R32_UINT_PACK32: return Format::R32U;
            case gli::FORMAT_R16_SFLOAT_PACK16: return Format::R16F;
            case gli::FORMAT_R32_SFLOAT_PACK32: return Format::R32F;

            case gli::FORMAT_RG8_SNORM_PACK8: return Format::RG8_SNorm;
            case gli::FORMAT_RG8_UNORM_PACK8: return Format::RG8_UNorm;
            case gli::FORMAT_RG16_SNORM_PACK16: return Format::RG16_SNorm;
            case gli::FORMAT_RG16_UNORM_PACK16: return Format::RG16_UNorm;
            case gli::FORMAT_RG8_SINT_PACK8: return Format::RG8I;
            case gli::FORMAT_RG8_UINT_PACK8: return Format::RG8U;
            case gli::FORMAT_RG16_SINT_PACK16: return Format::RG16I;
            case gli::FORMAT_RG16_UINT_PACK16: return Format::RG16U;
            case gli::FORMAT_RG32_SINT_PACK32: return Format::RG32I;
            case gli::FORMAT_RG32_UINT_PACK32: return Format::RG32U;
            case gli::FORMAT_RG16_SFLOAT_PACK16: return Format::RG16F;
            case gli::FORMAT_RG32_SFLOAT_PACK32: return Format::RG32F;

            case gli::FORMAT_RGB8_SNORM_PACK8: return Format::RGB8_SNorm;
            case gli::FORMAT_RGB8_UNORM_PACK8: return Format::RGB8_UNorm;
            case gli::FORMAT_RGB16_SNORM_PACK16: return Format::RGB16_SNorm;
            case gli::FORMAT_RGB16_UNORM_PACK16: return Format::RGB16_UNorm;
            case gli::FORMAT_BGR8_UNORM_PACK8: return Format::RGB16_UNorm;
            case gli::FORMAT_RGB8_SINT_PACK8: return Format::RGB8I;
            case gli::FORMAT_RGB8_UINT_PACK8: return Format::RGB8U;
            case gli::FORMAT_RGB16_SINT_PACK16: return Format::RGB16I;
            case gli::FORMAT_RGB16_UINT_PACK16: return Format::RGB16U;
            case gli::FORMAT_RGB32_SINT_PACK32: return Format::RGB32I;
            case gli::FORMAT_RGB32_UINT_PACK32: return Format::RGB32U;
            case gli::FORMAT_RGB16_SFLOAT_PACK16: return Format::RGB16F;
            case gli::FORMAT_RGB32_SFLOAT_PACK32: return Format::RGB32F;
            case gli::FORMAT_RGB8_SRGB_PACK8: return Format::sRGB8;
            case gli::FORMAT_BGR8_SRGB_PACK8: return Format::sBGR8;

            case gli::FORMAT_RGBA8_SNORM_PACK8: return Format::RGBA8_SNorm;
            case gli::FORMAT_RGBA8_UNORM_PACK8: return Format::RGBA8_UNorm;
            case gli::FORMAT_RGBA16_SNORM_PACK16: return Format::RGBA16_SNorm;
            case gli::FORMAT_RGBA16_UNORM_PACK16: return Format::RGBA16_UNorm;
            case gli::FORMAT_BGRA8_UNORM_PACK8: return Format::RGBA16_UNorm;
            case gli::FORMAT_RGBA8_SINT_PACK8: return Format::RGBA8I;
            case gli::FORMAT_RGBA8_UINT_PACK8: return Format::RGBA8U;
            case gli::FORMAT_RGBA16_SINT_PACK16: return Format::RGBA16I;
            case gli::FORMAT_RGBA16_UINT_PACK16: return Format::RGBA16U;
            case gli::FORMAT_RGBA32_SINT_PACK32: return Format::RGBA32I;
            case gli::FORMAT_RGBA32_UINT_PACK32: return Format::RGBA32U;
            case gli::FORMAT_RGBA16_SFLOAT_PACK16: return Format::RGBA16F;
            case gli::FORMAT_RGBA32_SFLOAT_PACK32: return Format::RGBA32F;
            case gli::FORMAT_RGBA8_SRGB_PACK8: return Format::sRGBA8;
            case gli::FORMAT_BGRA8_SRGB_PACK8: return Format::sBGRA8;
            default: break;
        }

        return Format::Undefined;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadKTX(core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        auto image =
            gli::load_ktx(reinterpret_cast<const char *>(std::data(data)), std::size(data));

        const auto faces      = core::as<core::UInt32>(image.faces());
        const auto layers     = core::as<core::UInt32>(image.layers());
        const auto mip_levels = core::as<core::UInt32>(image.levels());
        const auto format     = toStormFormat(image.format());

        if (format == Format::Undefined)
            return core::Unexpected { Error { .reason    = Reason::Failed_To_Parse,
                                              .str_error = "Unsupported pixel format" } };

        auto image_memory = core::ByteArray {};
        image_memory.resize(image.size());

        std::ranges::copy(core::toConstByteSpan(image.data(), image.size()),
                          std::begin(image_memory));

        auto image_data = image::Image::ImageData {};

        image_data.extent = core::ExtentI { image.extent().x, image.extent().y, image.extent().z };
        image_data.channel_count     = getChannelCountFor(format);
        image_data.bytes_per_channel = getSizeof(format);
        image_data.mip_levels        = mip_levels;
        image_data.faces             = faces;
        image_data.layers            = layers;
        image_data.data              = std::move(image_memory);
        image_data.format            = format;

        return Image { std::move(image_data) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveKTX(const image::Image &image, const std::filesystem::path &filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto result = saveKTX(image);

        if (!result) return core::Unexpected { result.error() };

        auto &output = *result;

        auto stream = std::ofstream { filepath, std::ios::binary };
        stream.write(reinterpret_cast<const char *>(std::data(output)), std::size(output));

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveKTX([[maybe_unused]] const image::Image &image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        return tl::unexpected { Error { .reason    = Reason::Not_Implemented,
                                        .str_error = "save to file" } };
    }
} // namespace stormkit::image::details
