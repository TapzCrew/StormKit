// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <queue>
#include <vector>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

#include <storm/entities/Entity.hpp>

namespace storm::entities {
    struct Message {
        core::UInt32 id;
        std::vector<Entity> entities;
    };

    class STORMKIT_PUBLIC MessageBus: public core::NonCopyable {
      public:
        MessageBus();
        ~MessageBus();

        MessageBus(MessageBus &&);
        MessageBus &operator=(MessageBus &&);

        void push(Message &&message);
        const Message &top() const;
        void pop();

        inline bool empty() const noexcept { return m_messages.empty(); }

      private:
        std::queue<Message> m_messages;
    };
} // namespace storm::entities
