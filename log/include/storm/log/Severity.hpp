// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Flags.hpp>
#include <storm/core/Strings.hpp>
#include <storm/core/Format.hpp>

namespace storm::log {
    enum class Severity { Info, Warning, Error, Fatal, Debug };

    constexpr auto toString(Severity severity) noexcept -> std::string_view {
        switch (severity) {
            case Severity::Info: return "Information";
            case Severity::Warning: return "Warning";
            case Severity::Error: return "Error";
            case Severity::Fatal: return "Fatal";
            case Severity::Debug: return "Debug";
        }

        return "Unkown severity";
    }
} // namespace storm::log

FLAG_ENUM(storm::log::Severity)

CUSTOM_FORMAT_STRING_VIEW(storm::log::Severity, storm::log::toString(data));

