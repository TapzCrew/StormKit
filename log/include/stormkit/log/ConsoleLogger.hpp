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

#include <stormkit/log/Logger.hpp>

namespace stormkit::log {
    class STORMKIT_PUBLIC ConsoleLogger final: public Logger {
      public:
        explicit ConsoleLogger(LogClock::time_point start) noexcept;
        ConsoleLogger(LogClock::time_point start, Severity log_level) noexcept;

        ~ConsoleLogger() override;

        auto write(Severity severity, const Module& module, const char *string) -> void override;
        auto flush() noexcept -> void override;
    };
    DECLARE_PTR_AND_REF(ConsoleLogger)
} // namespace stormkit::log
