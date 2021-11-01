// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstdint>
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Strings.hpp>

namespace storm::window {
    struct VideoSettings {
        core::Extentu size;
        core::UInt8 bpp  = 32u;
        core::UInt16 dpi = 1u;

        [[nodiscard]] constexpr inline auto operator==(const VideoSettings &other) const noexcept
            -> bool {
            return size.width == other.size.width && size.height == other.size.height &&
                   bpp == other.bpp && dpi == other.dpi;
        }

        [[nodiscard]] constexpr inline auto operator!=(const VideoSettings &other) const noexcept
            -> bool {
            return !operator==(other);
        }

        [[nodiscard]] constexpr inline auto operator>(const VideoSettings &other) const noexcept
            -> bool {
            return (size.width * size.height * size.depth) >
                   (other.size.width * other.size.height * other.size.height);
        }
    };

} // namespace storm::window
