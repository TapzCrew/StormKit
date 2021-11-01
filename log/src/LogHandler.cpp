// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/ConsoleLogger.hpp>
#include <storm/log/LogHandler.hpp>

using namespace storm::log;

void LogHandler::setupLogger(LoggerOwnedPtr &&ptr) {
    m_logger = std::move(ptr);
}

void LogHandler::setupDefaultLogger() {
    setupLogger<ConsoleLogger>();
}

Logger &LogHandler::logger() {
    return *m_logger;
}
