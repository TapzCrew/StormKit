// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    void Logger::setLogLevel(Severity log_level) { m_log_level = log_level; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const LogClock::time_point &Logger::startTime() const noexcept { return m_start_time; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const Severity &Logger::logLevel() const noexcept { return m_log_level; }
} // namespace storm::log
