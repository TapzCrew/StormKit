// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "HDRImage.mpp"

namespace stormkit::image::details {
    template<class E>
    using Unexpected = core::Unexpected<E>;
    using Error      = image::Image::Error;
    using Reason     = image::Image::Error::Reason;

    /////////////////////////////////////
    /////////////////////////////////////
    auto loadHDR([[maybe_unused]] core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error> {
        return core::Unexpected { Error { .reason    = Reason::Not_Implemented,
                                          .str_error = "loader from file" } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto saveHDR(const image::Image &image, const std::filesystem::path &filepath) noexcept
        -> core::Expected<void, image::Image::Error> {
        auto result = saveHDR(image);

        if (!result) return core::Unexpected { result.error() };

        auto &output = *result;

        auto stream = std::ofstream { filepath, std::ios::binary };
        stream.write(reinterpret_cast<const char *>(std::data(output)), std::size(output));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    [[nodiscard]] auto saveHDR([[maybe_unused]] const image::Image &image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error> {
        return tl::unexpected { Error { .reason    = Reason::Not_Implemented,
                                        .str_error = "save to file" } };
    }
} // namespace stormkit::image::details
