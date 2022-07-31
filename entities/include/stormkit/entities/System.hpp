// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>


#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <string>
#include <string_view>
#include <vector>


#include <stormkit/core/HashMap.hpp>
#include <stormkit/core/Types.hpp>


#include <stormkit/entities/Component.hpp>
#include <stormkit/entities/Entity.hpp>

namespace stormkit::entities {
    class EntityManager;
    class STORMKIT_PUBLIC System {
      public:
        using ComponentTypes = core::HashSet<Component::Type>;

        System(EntityManager &manager, core::UInt32 priority, ComponentTypes &&types);
        System(EntityManager &manager, core::UInt32 priority, const ComponentTypes &types);

        System(const System &) = delete;
        auto operator=(const System &) -> System & = delete;

        System(System &&) noexcept;
        auto operator=(System &&) noexcept -> System &;

        virtual ~System();

        virtual auto preUpdate() -> void;
        virtual auto update(core::Secondf delta) -> void = 0;
        virtual auto postUpdate() -> void;

        [[nodiscard]] auto priority() const noexcept -> core::UInt32;
        [[nodiscard]] auto componentsUsed() const noexcept -> const ComponentTypes &;

        auto addEntity(Entity e) -> void;
        auto removeEntity(Entity e) -> void;

        struct Predicate {
            inline bool operator()(const SystemOwnedPtr &s1,
                                   const SystemOwnedPtr &s2) const noexcept {
                return s1->priority() < s2->priority();
            }
        };

      protected:
        virtual auto onMessageReceived(const Message &message) -> void = 0;

        EntityManager *m_manager;
        core::HashSet<Entity> m_entities;

        friend class EntityManager;

      private:
        core::UInt32 m_priority;
        ComponentTypes m_types;
    };
    DECLARE_PTR_AND_REF(System)
} // namespace stormkit::entities

#include "System.inl"