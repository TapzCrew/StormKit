/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

using namespace storm;
using namespace storm::image;

std::optional<std::string> Image::loadHDR(core::ByteConstSpan data) noexcept {
    return std::nullopt;
}

std::optional<std::string> Image::saveHDR(const std::filesystem::path &filepath) const noexcept {
    auto _filename = filepath;

    return std::nullopt;
}

std::optional<std::string> Image::saveHDR([[maybe_unused]] core::ByteArray &output) const noexcept {
    return std::nullopt;
}
