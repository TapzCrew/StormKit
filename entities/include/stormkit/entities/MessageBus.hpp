// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>


#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <string>
#include <string_view>
#include <vector>


#include <stormkit/core/Types.hpp>


#include <stormkit/entities/Entity.hpp>

namespace stormkit::entities {
    struct Message {
        core::UInt32 id;
        std::vector<Entity> entities;
    };

    class STORMKIT_PUBLIC MessageBus {
      public:
        MessageBus();
        ~MessageBus();

        MessageBus(const MessageBus &) = delete;
        auto operator=(const MessageBus &) -> MessageBus & = delete;

        MessageBus(MessageBus &&);
        auto operator=(MessageBus &&) -> MessageBus &;

        auto push(Message &&message) -> void;
        [[nodiscard]] auto top() const -> const Message &;
        auto pop() -> void;

        [[nodiscard]] auto empty() const noexcept -> bool;

      private:
        std::queue<Message> m_messages;
    };
    DECLARE_PTR_AND_REF(MessageBus)
} // namespace stormkit::entities

#include "MessageBus.inl"