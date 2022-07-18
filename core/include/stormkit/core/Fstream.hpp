// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <fstream>

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::core {
    [[nodiscard]] auto read(std::istream& stream, std::streamsize size) noexcept -> core::ByteArray;
    auto read(std::istream& stream, core::ByteSpan output) noexcept -> void;

    auto write(std::ostream& stream, core::ByteConstSpan data) noexcept -> void;
} // namespace stormkit::core

#include "Fstream.inl"
