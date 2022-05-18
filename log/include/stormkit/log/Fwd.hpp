// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>


#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::log {
    class Logger;
    DECLARE_PTR_AND_REF(Logger)

    class ConsoleLogger;
    DECLARE_PTR_AND_REF(ConsoleLogger)

    class FileLogger;
    DECLARE_PTR_AND_REF(FileLogger)
} // namespace stormkit::log
