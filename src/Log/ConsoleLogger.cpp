// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <cstdio>

module stormkit.Log;

import std;

import stormkit.Core;

import :LogColorizer;

using namespace std::literals;

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start) noexcept
        : Logger { std::move(start) } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::ConsoleLogger(LogClock::time_point start, Severity log_level) noexcept
        : Logger { std::move(start), log_level } {
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    ConsoleLogger::~ConsoleLogger() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::write(Severity severity, const Module& m, const char* string) -> void {
        const auto now  = LogClock::now();
        const auto time = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);

        const auto str = [&]() {
            if (std::empty(m.name)) return std::format("[{}, {:%S}]", severity, time);
            else
                return std::format("[{}, {:%S}, {}]", severity, time, m.name);
        }();

        const auto to_stderr = severity == Severity::Error or severity == Severity::Fatal;
        auto       output    = (to_stderr) ? stderr : stdout;

        // not yet
        /*
        auto state          = std::mbstate_t {};
        std::string out_str = std::string { MB_LEN_MAX };
        for (const auto &c : str) std::c8rtomb(std::data(out_str), c, &state);

        state                  = std::mbstate_t {};
        std::string out_string = std::string { MB_LEN_MAX };
        for (const auto &c : string) { std::c8rtomb(std::data(out_string), c, &state); }*/

        details::colorifyBegin(severity, to_stderr);
        std::print(output, "{}", str);
        details::colorifyEnd(to_stderr);
        std::println(output, " {}", string);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    auto ConsoleLogger::flush() noexcept -> void {
        std::fflush(stdout);
    }
} // namespace stormkit::log
