// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/Expected.hpp>

#include <stormkit/image/Image.hpp>

namespace stormkit::image::details {
    [[nodiscard]] auto loadTGA(core::ByteConstSpan data) noexcept
        -> core::Expected<image::Image, image::Image::Error>;

    [[nodiscard]] auto saveTGA(const image::Image &image,
                               const std::filesystem::path &filepath) noexcept
        -> core::Expected<void, image::Image::Error>;

    [[nodiscard]] auto saveTGA(const image::Image &image) noexcept
        -> core::Expected<core::ByteArray, image::Image::Error>;
} // namespace stormkit::image::details
