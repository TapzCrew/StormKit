// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::setLogLevel(Severity log_level) noexcept -> void {
        m_log_level = log_level;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::startTime() const noexcept -> const LogClock::time_point& {
        return m_start_time;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::logLevel() const noexcept -> const Severity& {
        return m_log_level;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    auto Logger::createLoggerInstance(Args&&...param_args) -> T {
        using LogClock = std::chrono::high_resolution_clock;
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        return T { std::move(time_point), std::forward<Args>(param_args)... };
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    auto Logger::allocateLoggerInstance(Args&&...param_args) -> std::unique_ptr<T> {
        using LogClock = std::chrono::high_resolution_clock;
        static_assert(std::is_base_of<Logger, T>::value, "T must inherit Logger");

        auto time_point = LogClock::now();

        return std::make_unique<T>(std::move(time_point), std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::log(Severity severity,
                     const Module& m,
                     std::string_view format_string,
                     Args&&...param_args) -> void {
        STORMKIT_EXPECTS(m_logger);

        const auto format  = format_string;
        auto memory_buffer = std::string {};
        core::vformat_to(std::back_inserter(memory_buffer),
                         format,
                         core::make_format_args(std::forward<Args>(param_args)...));

        m_logger->write(severity, m, std::data(memory_buffer));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::log(Severity severity, std::string_view format_string, Args&&...param_args)
        -> void {
        log(severity, ""_module, format_string, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::dlog(Args&&...param_args) -> void {
        log(Severity::Debug, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::ilog(Args&&...param_args) -> void {
        log(Severity::Info, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::wlog(Args&&...param_args) -> void {
        log(Severity::Warning, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::elog(Args&&...param_args) -> void {
        log(Severity::Error, std::forward<Args>(param_args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename... Args>
    auto Logger::flog(Args&&...param_args) -> void {
        log(Severity::Fatal, std::forward<Args>(param_args)...);
    }
} // namespace stormkit::log
