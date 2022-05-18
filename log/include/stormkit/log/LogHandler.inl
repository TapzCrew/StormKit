// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    auto LogHandler::setupLogger(Args &&...param_args) -> void {
        using LogClock = std::chrono::high_resolution_clock;
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        if (m_logger) time_point = m_logger->startTime();

        auto logger = std::make_unique<T>(std::move(time_point), std::forward<Args>(param_args)...);

        setupLogger(std::move(logger));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::log(Severity severity,
                         Module m,
                         std::string_view format_string,
                         Args &&...param_args) -> void {
        if (!m_logger) setupDefaultLogger();

        auto memory_buffer = std::string {};
        core::vformat_to(std::back_inserter(memory_buffer),
                         core::runtime(format_string),
                         core::make_format_args(std::forward<Args>(param_args)...));

        logger().write(severity, m, std::data(memory_buffer));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::log(Severity severity, std::string_view format_string, Args &&...param_args)
        -> void {
        log(severity, ""_module, format_string, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::dlog(Args &&...param_args) -> void {
        log(Severity::Debug, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::ilog(Args &&...param_args) -> void {
        log(Severity::Info, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::wlog(Args &&...param_args) -> void {
        log(Severity::Warning, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::elog(Args &&...param_args) -> void {
        log(Severity::Error, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto LogHandler::flog(Args &&...param_args) -> void {
        log(Severity::Fatal, std::forward<Args>(param_args)...);
    }

} // namespace stormkit::log
