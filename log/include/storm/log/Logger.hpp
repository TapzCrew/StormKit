// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <gsl/string_span>

#include <storm/core/Platform.hpp>

#include <storm/log/Fwd.hpp>
#include <storm/log/Module.hpp>
#include <storm/log/Severity.hpp>

namespace storm::log {
    class STORMKIT_PUBLIC Logger {
      public:
        explicit Logger(LogClock::time_point start, Severity log_level = DEFAULT_SEVERITY);
        virtual ~Logger();

        Logger(Logger &&);
        Logger &operator=(Logger &&);

        Logger(const Logger &);
        Logger &operator=(const Logger &);

        virtual void write(Severity severity, Module module, const char *string) = 0;
        virtual void flush()                                                     = 0;

        inline void setLogLevel(Severity log_level);

        inline const LogClock::time_point &startTime() const noexcept;
        inline const Severity &logLevel() const noexcept;

      protected:
#ifdef STORMKIT_BUILD_DEBUG
        static constexpr const auto DEFAULT_SEVERITY = Severity::Info | Severity::Debug |
                                                       Severity::Error | Severity::Fatal |
                                                       Severity::Warning;
#else
        static constexpr const auto DEFAULT_SEVERITY =
            Severity::Info | Severity::Error | Severity::Fatal;
#endif

        LogClock::time_point m_start_time;
        Severity m_log_level;

        friend class LogHandler;
    };
} // namespace storm::log

#include "Logger.inl"
