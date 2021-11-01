// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>
#include <queue>
#include <storm/core/Strings.hpp>

namespace storm::log {
    template<class T, typename... Args>
    void LogHandler::setupLogger(Args &&...param_args) {
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        if (m_logger) time_point = m_logger->startTime();

        auto logger = std::make_unique<T>(std::move(time_point), std::forward<Args>(param_args)...);

        setupLogger(std::move(logger));
    }

    template<typename... Args>
    void LogHandler::log(Severity severity,
                         Module module,
                         std::string_view format_string,
                         Args &&...param_args) {
        if (!m_logger) setupDefaultLogger();

        auto memory_buffer = std::string{};
        core::format_to(std::back_inserter(memory_buffer), format_string, std::forward<Args>(param_args)...);

        logger().write(severity, module, std::data(memory_buffer));
    }

    template<typename... Args>
    void LogHandler::log(Severity severity, std::string_view format_string, Args &&...param_args) {
        log(severity, ""_module, std::move(format_string), std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    void LogHandler::dlog(Args &&...param_args) {
        log(Severity::Debug, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    void LogHandler::ilog(Args &&...param_args) {
        log(Severity::Info, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    void LogHandler::wlog(Args &&...param_args) {
        log(Severity::Warning, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    void LogHandler::elog(Args &&...param_args) {
        log(Severity::Error, std::forward<Args>(param_args)...);
    }

    template<typename... Args>
    void LogHandler::flog(Args &&...param_args) {
        log(Severity::Fatal, std::forward<Args>(param_args)...);
    }

} // namespace storm::log
