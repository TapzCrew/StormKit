// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <ranges>

#include <stormkit/core/Coroutines.mpp>
#include <stormkit/core/Fstream.mpp>

#include "PPMImage.mpp"

using namespace std::literals;

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;
    using Format     = image::Image::Format;

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadPPM([[maybe_unused]] core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        return core::makeUnexpected(
            Error { .reason = Reason::Not_Implemented, .str_error = "loader from memory" });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto savePPM(const image::Image& image,
                 image::Image::CodecArgs args,
                 const std::filesystem::path& filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto result = savePPM(image, args);

        if (!result) return core::makeUnexpected(result.error());

        auto stream = std::ofstream {};

        if (args == image::Image::CodecArgs::Ascii) stream.open(filepath);
        else
            stream.open(filepath, std::ios::binary);

        auto& output = *result;

        core::write(stream, core::toConstByteSpan(output));

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto savePPM(const image::Image& image, image::Image::CodecArgs args) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        const auto output_image = image.toFormat(Format::RGB8_UNorm);
        const auto& data        = output_image.imageData();

        auto output = core::ByteArray {};

        if (args == image::Image::CodecArgs::Ascii) {
            auto result =
                core::format("P3\n{}\n{}\n255\n"sv, data.extent.width, data.extent.height);

            const auto& extent = output_image.extent();
            for (auto [i, j] : core::generateIndices(extent.height, extent.width)) {
                auto pixel = output_image.pixel(i * output_image.extent().width + j);

                result += core::format("{} {} {}\n"sv, pixel[0], pixel[1], pixel[2]);

                if (j == extent.width) result += '\n';
            }

            output.reserve(std::size(result));
            std::ranges::copy(core::toConstByteSpan(result), std::back_inserter(output));
        } else if (args == image::Image::CodecArgs::Binary) {
            auto header =
                core::format("P3\n{}\n{}\n255\n"sv, data.extent.width, data.extent.height);
            output.reserve(std::size(output) + std::size(output_image));

            std::ranges::copy(core::toConstByteSpan(header), std::back_inserter(output));
            std::ranges::copy(output_image, std::back_inserter(output));
        }

        return output;
    }
} // namespace stormkit::image::details
