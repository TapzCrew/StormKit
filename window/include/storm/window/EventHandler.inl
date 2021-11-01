// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EventHandler::addCallback(EventType event_type, Callback callback) -> void {
        m_callback[event_type].emplace_back(std::move(callback));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EventHandler::clearCallbacks(EventType event_type) -> void {
        m_callback[event_type].clear();
    }
} // namespace storm::window
