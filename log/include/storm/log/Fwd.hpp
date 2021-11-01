// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Format.hpp>

#include <chrono>
#include <storm/core/Memory.hpp>

namespace storm::log {
    using LogClock = std::chrono::high_resolution_clock;

    class Logger;
    DECLARE_PTR_AND_REF(Logger)

    class ConsoleLogger;
    DECLARE_PTR_AND_REF(ConsoleLogger)

    class FileLogger;
    DECLARE_PTR_AND_REF(FileLogger)

    class LogColorizer;
    DECLARE_PTR_AND_REF(LogColorizer)
} // namespace storm::log
