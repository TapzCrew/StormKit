// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/Memory.hpp>

#include <stormkit/log/Module.hpp>
#include <stormkit/log/Severity.hpp>

namespace stormkit::log {
    class STORMKIT_PUBLIC Logger {
      public:
        using LogClock = std::chrono::high_resolution_clock;
        explicit Logger(LogClock::time_point start) noexcept;
        Logger(LogClock::time_point start, Severity log_level) noexcept;
        virtual ~Logger();

        virtual auto write(Severity severity, const Module& module, const char *string) -> void = 0;
        virtual auto flush() -> void                                                            = 0;

        auto setLogLevel(Severity log_level) noexcept -> void;

        [[nodiscard]] auto startTime() const noexcept -> const LogClock::time_point&;
        [[nodiscard]] auto logLevel() const noexcept -> const Severity&;

        template<class T, typename... Args>
        [[nodiscard]] static auto createLoggerInstance(Args&&...param_args) -> T;

        template<class T, typename... Args>
        [[nodiscard]] static auto allocateLoggerInstance(Args&&...param_args) -> std::unique_ptr<T>;

        template<typename... Args>
        static auto log(Severity severity,
                        const Module& module,
                        std::string_view format_string,
                        Args&&...param_args) -> void;

        template<typename... Args>
        static auto log(Severity severity, std::string_view format_string, Args&&...param_args)
            -> void;

        template<typename... Args>
        static auto dlog(Args&&...param_args) -> void;

        template<typename... Args>
        static auto ilog(Args&&...param_args) -> void;

        template<typename... Args>
        static auto wlog(Args&&...param_args) -> void;

        template<typename... Args>
        static auto elog(Args&&...param_args) -> void;

        template<typename... Args>
        static auto flog(Args&&...param_args) -> void;

        static auto instance() -> Logger&;

      protected:
        LogClock::time_point m_start_time;
        Severity m_log_level;

        static Logger *m_logger;
    };
    DECLARE_PTR_AND_REF(Logger)
} // namespace stormkit::log

#include "Logger.inl"
#include "Module.inl"
