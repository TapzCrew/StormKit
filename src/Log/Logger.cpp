// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Log;

import std;

import stormkit.Core;

import :Logger;
import :Severity;

namespace stormkit::log {
#ifdef STORMKIT_BUILD_DEBUG
    static constexpr auto DEFAULT_SEVERITY =
        Severity::Info | Severity::Debug | Severity::Error | Severity::Fatal | Severity::Warning;
#else
    static constexpr auto DEFAULT_SEVERITY = Severity::Info | Severity::Error | Severity::Fatal;
#endif

    static Logger *logger = nullptr;

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time) noexcept
        : Logger { std::move(start_time), DEFAULT_SEVERITY } {
        core::expects(not logger);

        logger = this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time, Severity log_level) noexcept
        : m_start_time { std::move(start_time) }, m_log_level { log_level } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::~Logger() {
        logger = nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Logger::hasLogger() noexcept -> bool {
        if (logger) [[likely]]
            return true;
        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Logger::instance() noexcept -> Logger& {
        core::expects(logger);

        return *logger;
    }
} // namespace stormkit::log