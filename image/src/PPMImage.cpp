/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

using namespace std::literals;

using namespace storm;
using namespace storm::image;

std::optional<std::string> Image::loadPPM(core::ByteConstSpan data) noexcept {
    return std::nullopt;
}

std::optional<std::string> Image::savePPM(const std::filesystem::path &filepath,
                                          CodecArgs args) const noexcept {
    auto _filename = filepath;

    auto output = core::ByteArray{};
    if(auto err = savePPM(output, args); err)
        return err;

    auto stream = std::ofstream{};

    if(args == CodecArgs::Ascii)
        stream.open(filepath);
    else stream.open(filepath, std::ios::binary);

    stream.write(reinterpret_cast<const char *>(std::data(output)), std::size(output));

    return std::nullopt;
}

std::optional<std::string> Image::savePPM(core::ByteArray &output,
                                          CodecArgs args) const noexcept {
    auto image = toFormat(Format::RGB8_UNorm);

    if(args == CodecArgs::Ascii) {
        auto result = core::format("P3\n{}\n{}\n255\n"sv, m_extent.width, m_extent.height);

        for(auto i = 0u; i < image.extent().height; ++i) {
            for(auto j = 0u; j < image.extent().width; ++j) {
                auto pixel = image.pixel(i * image.extent().width + j);

                result += core::format("{} {} {}\n"sv, pixel[0], pixel[1], pixel[2]);
            }

            result += '\n';
        }

        output.reserve(std::size(result));
        core::ranges::copy(core::toConstByteSpan(result), std::back_inserter(output));
    } else if(args == CodecArgs::Binary) {
        auto header = core::format("P3\n{}\n{}\n255\n"sv, m_extent.width, m_extent.height);
        output.reserve(std::size(output) + std::size(image.data()));

        core::ranges::copy(core::toConstByteSpan(header), std::back_inserter(output));
        core::ranges::copy(image.data(), std::back_inserter(output));
    }

    return std::nullopt;
}
