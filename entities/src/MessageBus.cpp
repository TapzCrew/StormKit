// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution


#include <stormkit/entities/MessageBus.hpp>

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    MessageBus::MessageBus() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    MessageBus::~MessageBus() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    MessageBus::MessageBus(MessageBus &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    MessageBus &MessageBus::operator=(MessageBus &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto MessageBus::push(Message &&message) -> void { m_messages.emplace(std::move(message)); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto MessageBus::top() const -> const Message & { return m_messages.front(); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto MessageBus::pop() -> void { m_messages.pop(); }
} // namespace stormkit::entities
