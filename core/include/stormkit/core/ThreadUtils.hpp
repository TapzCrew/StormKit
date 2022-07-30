// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <string_view>
#include <thread>

namespace stormkit::core {
    STORMKIT_PUBLIC auto setCurrentThreadName(std::string_view name) -> void;
    STORMKIT_PUBLIC auto setThreadName(std::thread& thread, std::string_view name) -> void;
} // namespace stormkit::core
