// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.loghandler;

// clang-format off
import stormkit.log.consolelogger;
// clang-format on
#else
    /////////// - StormKit::log - ///////////
    #include <stormkit/log/ConsoleLogger.mpp>
    #include <stormkit/log/LogHandler.mpp>
#endif

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
