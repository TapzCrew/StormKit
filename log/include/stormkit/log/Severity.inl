// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Severity.mpp"

namespace stormkit::log {
    namespace details {
        constexpr static auto SEVERITY_TO_STRING = [] {
            using namespace std::literals;

            return core::makeFrozenMap<Severity, std::string_view>({
                { Severity::Info, "Info"sv },
                { Severity::Warning, "Warning"sv },
                { Severity::Error, "Error"sv },
                { Severity::Fatal, "Fatal"sv },
                { Severity::Debug, "Debug"sv },
            });
        }();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto toString(Severity severity) noexcept -> std::string_view {
        const auto it = details::SEVERITY_TO_STRING.find(severity);
        STORMKIT_CONSTEXPR_EXPECTS(it != std::ranges::cend(details::SEVERITY_TO_STRING));

        return it->second;
    }
} // namespace stormkit::log
