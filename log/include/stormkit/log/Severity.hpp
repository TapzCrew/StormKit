// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FlagsMacro.hpp>
#include <stormkit/core/FormatMacro.hpp>

#include <algorithm>
#include <string_view>

#include <stormkit/core/Flags.hpp>
#include <stormkit/core/Format.hpp>
#include <stormkit/core/FrozenMap.hpp>

namespace stormkit::log {
    enum class Severity { Info, Warning, Error, Fatal, Debug };

    [[nodiscard]] constexpr auto toString(Severity severity) noexcept -> std::string_view;
} // namespace stormkit::log

FLAG_ENUM(stormkit::log::Severity)

CUSTOM_FORMAT_STRING_VIEW(stormkit::log::Severity, stormkit::log::toString(data));

#include "Severity.inl"
