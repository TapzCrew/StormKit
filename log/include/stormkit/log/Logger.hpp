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

        virtual auto write(Severity severity, const Module &module, const char *string) -> void = 0;
        virtual auto flush() -> void                                                            = 0;

        auto setLogLevel(Severity log_level) noexcept -> void;

        [[nodiscard]] auto startTime() const noexcept -> const LogClock::time_point &;
        [[nodiscard]] auto logLevel() const noexcept -> const Severity &;

      protected:
        LogClock::time_point m_start_time;
        Severity m_log_level;

        friend class LogHandler;
    };
    DECLARE_PTR_AND_REF(Logger)
} // namespace stormkit::log

#include "Logger.inl"
