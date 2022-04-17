// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.logger;

// clang-format off
// clang-format on
#else
    /////////// - StormKit::log - ///////////
    #include <stormkit/log/Logger.mpp>
#endif

namespace stormkit::log {
#ifdef STORMKIT_BUILD_DEBUG
    constexpr auto DEFAULT_SEVERITY =
        Severity::Info | Severity::Debug | Severity::Error | Severity::Fatal | Severity::Warning;
#else
    constexpr auto DEFAULT_SEVERITY = Severity::Info | Severity::Error | Severity::Fatal;
#endif

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time) noexcept
        : Logger { std::move(start_time), DEFAULT_SEVERITY } {}

    /////////////////////////////////////
    /////////////////////////////////////
    Logger::Logger(LogClock::time_point start_time, Severity log_level) noexcept
        : m_start_time { std::move(start_time) }, m_log_level { log_level } {}
} // namespace stormkit::log
