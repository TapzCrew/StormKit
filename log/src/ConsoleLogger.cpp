// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.consolelogger;

// clang-format off
/////////// - STL - ///////////
#include <climits>
#include <clocale>
import <iostream>;
import <string_view>;

/////////// - StormKit::core - ///////////
import stormkit.core.format
import stormkit.core.strings

/////////// - StormKit::log - ///////////
import stormkit.log.details.logcolorizer;
// clang-format on
#else
/////////// - STL - ///////////
    #include <climits>
    #include <clocale>
    #include <iostream>
    #include <string_view>

    //////////// - StormKit::core - ///////////
    #include <stormkit/core/Format.mpp>
    #include <stormkit/core/Strings.mpp>

/////////// - StormKit::log - ///////////
    #include "LogColorizer.mpp"
    #include <stormkit/log/ConsoleLogger.mpp>
#endif

using namespace std::literals;

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start) noexcept
        : Logger { std::move(start) } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level) noexcept
        : Logger { std::move(start), log_level } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::write(Severity severity, Module m, const char *string) -> void {
        const auto now  = LogClock::now();
        const auto time = std::chrono::duration_cast<core::Secondf>(now - m_start_time);

        static constexpr auto LOG_LINE        = "[{0}, {1}s]"sv;
        static constexpr auto LOG_LINE_MODULE = "[{0}, {1}s, {2}]"sv;

        const auto str = [&severity, &time, &m]() {
            if (std::size(*m) == 0) return core::format(LOG_LINE, severity, time);
            else
                return core::format(LOG_LINE_MODULE, severity, time, *m);
        }();

        const auto to_stderr = severity == Severity::Error || severity == Severity::Fatal;
        auto &output         = (to_stderr) ? std::cerr : std::cout;

        // not yet
        /*
        auto state          = std::mbstate_t {};
        std::string out_str = std::string { MB_LEN_MAX };
        for (const auto &c : str) std::c8rtomb(std::data(out_str), c, &state);

        state                  = std::mbstate_t {};
        std::string out_string = std::string { MB_LEN_MAX };
        for (const auto &c : string) { std::c8rtomb(std::data(out_string), c, &state); }*/

        details::colorifyBegin(severity, to_stderr);
        output << str;
        details::colorifyEnd(to_stderr);
        output << " " << string << '\n' << std::flush;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::flush() noexcept -> void { std::cout.flush(); }
} // namespace stormkit::log
