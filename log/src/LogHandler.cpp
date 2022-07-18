// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/log/ConsoleLogger.hpp>
#include <stormkit/log/LogHandler.hpp>

namespace stormkit::log {
    /////////////////////////////////////
    /////////////////////////////////////
    auto LogHandler::setupLogger(LoggerOwnedPtr &&ptr) -> void { m_logger = std::move(ptr); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto LogHandler::setupDefaultLogger() -> void { setupLogger<ConsoleLogger>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto LogHandler::logger() -> Logger & { return *m_logger; }
} // namespace stormkit::log
