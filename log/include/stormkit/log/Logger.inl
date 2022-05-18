// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::log {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::setLogLevel(Severity log_level) noexcept -> void {
        m_log_level = log_level;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::startTime() const noexcept -> const LogClock::time_point & {
        return m_start_time;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Logger::logLevel() const noexcept -> const Severity & { return m_log_level; }
} // namespace stormkit::log
