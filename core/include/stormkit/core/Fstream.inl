// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    inline auto read(std::istream& stream, core::USize size) noexcept -> core::ByteArray {
        auto output = core::ByteArray { size };

        read(stream, output);

        return output;
    }

    template<core::USize size>
    inline auto read(std::istream& stream) noexcept -> core::ByteStaticArray<size> {
        auto output = core::ByteStaticArray<size> {};

        read(stream, output);

        return output;
    }

    inline auto read(std::istream& stream, core::ByteSpan output) noexcept -> void {
        stream.read(reinterpret_cast<char *>(std::data(output)), std::size(output));
    }

    inline auto write(std::ostream& stream, core::ByteConstSpan data) noexcept -> void {
        stream.write(reinterpret_cast<const char *>(std::data(data)), std::size(data));
    }
} // namespace stormkit::core
