/////////// - StormKit::image - ///////////
#include <storm/core/Ranges.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - STL - ///////////
#include <csetjmp>
#include <cstring>

/////////// - libJPEG - ///////////
#include <gli/gli.hpp>

using namespace storm;
using namespace storm::image;

namespace storm::image {
    constexpr auto toStormFormat(gli::format format) noexcept {
        switch (format) {
            case gli::FORMAT_R8_SNORM_PACK8: return Image::Format::R8_SNorm;
            case gli::FORMAT_R8_UNORM_PACK8: return Image::Format::R8_UNorm;
            case gli::FORMAT_R16_SNORM_PACK16: return Image::Format::R16_SNorm;
            case gli::FORMAT_R16_UNORM_PACK16: return Image::Format::R16_UNorm;
            case gli::FORMAT_R8_SINT_PACK8: return Image::Format::R8I;
            case gli::FORMAT_R8_UINT_PACK8: return Image::Format::R8U;
            case gli::FORMAT_R16_SINT_PACK16: return Image::Format::R16I;
            case gli::FORMAT_R16_UINT_PACK16: return Image::Format::R16U;
            case gli::FORMAT_R32_SINT_PACK32: return Image::Format::R32I;
            case gli::FORMAT_R32_UINT_PACK32: return Image::Format::R32U;
            case gli::FORMAT_R16_SFLOAT_PACK16: return Image::Format::R16F;
            case gli::FORMAT_R32_SFLOAT_PACK32: return Image::Format::R32F;

            case gli::FORMAT_RG8_SNORM_PACK8: return Image::Format::RG8_SNorm;
            case gli::FORMAT_RG8_UNORM_PACK8: return Image::Format::RG8_UNorm;
            case gli::FORMAT_RG16_SNORM_PACK16: return Image::Format::RG16_SNorm;
            case gli::FORMAT_RG16_UNORM_PACK16: return Image::Format::RG16_UNorm;
            case gli::FORMAT_RG8_SINT_PACK8: return Image::Format::RG8I;
            case gli::FORMAT_RG8_UINT_PACK8: return Image::Format::RG8U;
            case gli::FORMAT_RG16_SINT_PACK16: return Image::Format::RG16I;
            case gli::FORMAT_RG16_UINT_PACK16: return Image::Format::RG16U;
            case gli::FORMAT_RG32_SINT_PACK32: return Image::Format::RG32I;
            case gli::FORMAT_RG32_UINT_PACK32: return Image::Format::RG32U;
            case gli::FORMAT_RG16_SFLOAT_PACK16: return Image::Format::RG16F;
            case gli::FORMAT_RG32_SFLOAT_PACK32: return Image::Format::RG32F;

            case gli::FORMAT_RGB8_SNORM_PACK8: return Image::Format::RGB8_SNorm;
            case gli::FORMAT_RGB8_UNORM_PACK8: return Image::Format::RGB8_UNorm;
            case gli::FORMAT_RGB16_SNORM_PACK16: return Image::Format::RGB16_SNorm;
            case gli::FORMAT_RGB16_UNORM_PACK16: return Image::Format::RGB16_UNorm;
            case gli::FORMAT_BGR8_UNORM_PACK8: return Image::Format::RGB16_UNorm;
            case gli::FORMAT_RGB8_SINT_PACK8: return Image::Format::RGB8I;
            case gli::FORMAT_RGB8_UINT_PACK8: return Image::Format::RGB8U;
            case gli::FORMAT_RGB16_SINT_PACK16: return Image::Format::RGB16I;
            case gli::FORMAT_RGB16_UINT_PACK16: return Image::Format::RGB16U;
            case gli::FORMAT_RGB32_SINT_PACK32: return Image::Format::RGB32I;
            case gli::FORMAT_RGB32_UINT_PACK32: return Image::Format::RGB32U;
            case gli::FORMAT_RGB16_SFLOAT_PACK16: return Image::Format::RGB16F;
            case gli::FORMAT_RGB32_SFLOAT_PACK32: return Image::Format::RGB32F;
            case gli::FORMAT_RGB8_SRGB_PACK8: return Image::Format::sRGB8;
            case gli::FORMAT_BGR8_SRGB_PACK8: return Image::Format::sBGR8;

            case gli::FORMAT_RGBA8_SNORM_PACK8: return Image::Format::RGBA8_SNorm;
            case gli::FORMAT_RGBA8_UNORM_PACK8: return Image::Format::RGBA8_UNorm;
            case gli::FORMAT_RGBA16_SNORM_PACK16: return Image::Format::RGBA16_SNorm;
            case gli::FORMAT_RGBA16_UNORM_PACK16: return Image::Format::RGBA16_UNorm;
            case gli::FORMAT_BGRA8_UNORM_PACK8: return Image::Format::RGBA16_UNorm;
            case gli::FORMAT_RGBA8_SINT_PACK8: return Image::Format::RGBA8I;
            case gli::FORMAT_RGBA8_UINT_PACK8: return Image::Format::RGBA8U;
            case gli::FORMAT_RGBA16_SINT_PACK16: return Image::Format::RGBA16I;
            case gli::FORMAT_RGBA16_UINT_PACK16: return Image::Format::RGBA16U;
            case gli::FORMAT_RGBA32_SINT_PACK32: return Image::Format::RGBA32I;
            case gli::FORMAT_RGBA32_UINT_PACK32: return Image::Format::RGBA32U;
            case gli::FORMAT_RGBA16_SFLOAT_PACK16: return Image::Format::RGBA16F;
            case gli::FORMAT_RGBA32_SFLOAT_PACK32: return Image::Format::RGBA32F;
            case gli::FORMAT_RGBA8_SRGB_PACK8: return Image::Format::sRGBA8;
            case gli::FORMAT_BGRA8_SRGB_PACK8: return Image::Format::sBGRA8;
            default: return Image::Format::Undefined;
        }

        return Image::Format::Undefined;
    }
} // namespace storm::image

std::optional<std::string> Image::loadKTX(core::ByteConstSpan data) noexcept {
    auto image = gli::load_ktx(reinterpret_cast<const char *>(std::data(data)), std::size(data));

    const auto faces      = static_cast<core::UInt32>(image.faces());
    const auto layers     = static_cast<core::UInt32>(image.layers());
    const auto mip_levels = static_cast<core::UInt32>(image.levels());
    const auto format     = toStormFormat(image.format());

    if (format == Image::Format::Undefined) return "Unsupported pixel format";

    auto _data = core::ByteArray {};
    _data.resize(image.size());

    core::ranges::copy(core::toConstByteSpan(image.data(), image.size()), std::begin(_data));

    m_extent            = core::Extenti { image.extent().x, image.extent().y, image.extent().z };
    m_channel_count     = getChannelCountFor(format);
    m_bytes_per_channel = getArraySizeByChannelFor(format);
    m_mip_levels        = mip_levels;
    m_faces             = faces;
    m_layers            = layers;
    m_data              = std::move(_data);
    m_format            = format;

    return std::nullopt;
}

std::optional<std::string> Image::saveKTX(const std::filesystem::path &filepath) const noexcept {
    return std::nullopt;
}

std::optional<std::string> Image::saveKTX(core::ByteArray &output) const noexcept {
    return std::nullopt;
}
