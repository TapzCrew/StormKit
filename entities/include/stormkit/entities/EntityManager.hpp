// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <vector>

#include <stormkit/core/HashMap.hpp>
#include <stormkit/core/Memory.hpp>

#include <stormkit/entities/Component.hpp>
#include <stormkit/entities/Entity.hpp>
#include <stormkit/entities/System.hpp>

namespace stormkit::entities {
    class STORMKIT_PUBLIC EntityManager {
      public:
        static constexpr auto ADDED_ENTITY_MESSAGE_ID   = 1;
        static constexpr auto REMOVED_ENTITY_MESSAGE_ID = 2;

        explicit EntityManager();
        ~EntityManager();

        EntityManager(const EntityManager &) = delete;
        auto operator=(const EntityManager &) -> EntityManager & = delete;

        EntityManager(EntityManager &&);
        auto operator=(EntityManager &&) -> EntityManager &;

        auto makeEntity() -> Entity;
        auto destroyEntity(Entity entity) -> void;
        auto destroyAllEntities() -> void;
        auto hasEntity(Entity entity) const -> bool;
        auto hasComponent(Entity entity) const -> bool;

        template<typename T, typename... Args>
        auto addComponent(Entity entity, Args &&...args) -> T &;

        template<typename T>
        auto destroyComponent(Entity entity) -> void;

        template<typename T>
        auto hasComponent(Entity entity) const -> bool;

        auto hasComponent(Entity entity, Component::Type type) const -> bool;

        template<typename T>
        auto entitiesWithComponent() const -> std::vector<Entity>;

        template<typename T>
        auto getComponent(Entity entity) -> T &;

        template<typename T>
        auto getComponent(Entity entity) const -> const T &;

        auto components(Entity entity) -> std::vector<ComponentRef>;
        auto components(Entity entity) const -> std::vector<ComponentConstRef>;

        template<typename T>
        auto componentsOfType() -> std::vector<std::reference_wrapper<T>>;

        template<std::derived_from<System> T, typename... Args>
        auto addSystem(Args &&...args) -> T &;

        template<std::derived_from<System> T>
        auto hasSystem() const noexcept -> bool;

        template<std::derived_from<System> T>
        auto getSystem() -> T &;

        template<std::derived_from<System> T>
        auto getSystem() const -> const T &;

        auto step(core::Secondf delta) -> void;

        auto entityCount() const noexcept -> core::USize;

        // void commit(Entity e);

      private:
        auto purposeToSystems(Entity e) -> void;
        auto removeFromSystems(Entity e) -> void;
        auto getNeededEntities(System &system) -> void;

        Entity m_next_valid_entity = 1;

        core::HashSet<Entity> m_entities;
        std::set<SystemOwnedPtr, System::Predicate> m_systems;
        core::HashMap<Entity, core::HashMap<Component::Type, ComponentOwnedPtr>> m_components;

        core::HashSet<Entity> m_added_entities;
        core::HashSet<Entity> m_updated_entities;
        core::HashSet<Entity> m_removed_entities;

        MessageBusOwnedPtr m_message_bus;
    };
    DECLARE_PTR_AND_REF(EntityManager)
} // namespace stormkit::entities

#include "EntityManager.inl"