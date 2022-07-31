// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FlagsMacro.hpp>
#include <stormkit/core/FormatMacro.hpp>

#include <stormkit/core/Format.hpp>
#include <stormkit/core/Math.hpp>

namespace stormkit::wsi {
    struct Monitor {
        enum class Flags {
            None = 0,
            Primary
        };

        Flags flags = Flags::None;
        std::string name;

        std::vector<core::ExtentU> extents;

        void *handle = nullptr;

        [[nodiscard]] constexpr auto operator<=>(const Monitor& other) const noexcept
            -> std::strong_ordering;

        [[nodiscard]] constexpr auto operator==(const Monitor& other) const noexcept -> bool;
    };

    FORMATTER(stormkit::wsi::Monitor,
              "Monitor {{\n"
              "   flags: {},\n"
              "   name: {},\n"
              "   extents: {}\n"
              "}}",
              data.flags,
              data.name,
              data.extents);

    auto toString(Monitor::Flags flags) -> std::string;
} // namespace stormkit::wsi

CUSTOM_FORMAT(stormkit::wsi::Monitor::Flags, stormkit::wsi::toString(data))
CUSTOM_FORMAT(stormkit::wsi::Monitor, stormkit::wsi::toString(data))
FLAG_ENUM(stormkit::wsi::Monitor::Flags)

#include "Monitor.inl"
