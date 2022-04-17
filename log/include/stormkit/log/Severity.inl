// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
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
} // namespace stormkit::log
