// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.log.logger;

// clang-format off
import stormkit.entities.messagebus;
// clang-format on
#else
    /////////// - StormKit::log - ///////////
    #include <stormkit/entities/EntityManager.mpp>
    #include <stormkit/entities/MessageBus.mpp>
#endif

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    EntityManager::EntityManager() { m_message_bus = std::make_unique<MessageBus>(); }

    /////////////////////////////////////
    /////////////////////////////////////
    EntityManager::EntityManager(EntityManager &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::operator=(EntityManager &&) -> EntityManager & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    EntityManager::~EntityManager() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::makeEntity() -> Entity {
        const auto entity = m_next_valid_entity++;

        m_added_entities.emplace(entity);
        m_message_bus->push(Message { ADDED_ENTITY_MESSAGE_ID, { entity } });

        return entity;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::destroyEntity(Entity entity) -> void {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY);

        if (hasEntity(entity)) {
            m_removed_entities.emplace(entity);
            m_message_bus->push(Message { REMOVED_ENTITY_MESSAGE_ID, { entity } });
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::hasEntity(Entity entity) const -> bool {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY);

        auto it  = m_entities.find(entity);
        auto it2 = m_added_entities.find(entity);

        return it != m_entities.cend() || it2 != m_added_entities.cend();
    }

    auto EntityManager::hasComponent(Entity entity) const -> bool {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY);

        auto it = m_components.find(entity);

        if (it == m_components.cend()) return false;

        return true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::hasComponent(Entity entity, Component::Type type) const -> bool {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY && type != Component::INVALID_TYPE);

        auto it = std::find_if(m_components.at(entity).cbegin(),
                               m_components.at(entity).cend(),
                               [type](auto &i) {
                                   if (i.first == type) return true;

                                   return false;
                               });

        return (it != m_components.at(entity).cend());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::components(Entity entity) -> std::vector<ComponentRef> {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY);

        if (!hasEntity(entity) || !hasComponent(entity)) return {};

        auto vec = std::vector<ComponentRef> {};
        vec.reserve(std::size(m_components.at(entity)));
        for (auto &i : m_components.at(entity)) { vec.emplace_back(std::ref(*i.second)); }

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::components(Entity entity) const -> std::vector<ComponentConstRef> {
        STORMKIT_EXPECTS(entity != INVALID_ENTITY);

        if (!hasEntity(entity) || !hasComponent(entity)) return {};

        auto vec = std::vector<ComponentConstRef> {};
        vec.reserve(std::size(m_components.at(entity)));
        for (auto &i : m_components.at(entity)) { vec.emplace_back(std::cref(*i.second)); }

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::step(core::Secondf delta) -> void {
        for (auto entity : m_added_entities) { m_entities.emplace(entity); }
        m_added_entities.clear();

        for (auto entity : m_updated_entities) { purposeToSystems(entity); }
        m_updated_entities.clear();

        for (auto entity : m_removed_entities) {
            auto it = m_entities.find(entity);

            auto it_map = m_components.find(entity);
            if (hasComponent(entity)) m_components.erase(it_map);

            m_entities.erase(it);

            removeFromSystems(entity);
        }
        m_removed_entities.clear();

        while (!m_message_bus->empty()) {
            for (auto &system : m_systems) system->onMessageReceived(m_message_bus->top());
            m_message_bus->pop();
        }

        for (auto &system : m_systems) system->preUpdate();
        for (auto &system : m_systems) system->update(delta);
        for (auto &system : m_systems) system->postUpdate();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::purposeToSystems(Entity e) -> void {
        STORMKIT_EXPECTS(e != INVALID_ENTITY);

        for (auto &s : m_systems) {
            auto is_reliable = true;
            for (auto t : s->componentsUsed()) {
                if (!hasComponent(e, t)) {
                    is_reliable = false;
                    break;
                }
            }

            if (is_reliable) s->addEntity(e);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::removeFromSystems(Entity e) -> void {
        STORMKIT_EXPECTS(e != INVALID_ENTITY);

        for (auto &s : m_systems) { s->removeEntity(e); }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EntityManager::getNeededEntities(System &system) -> void {
        for (auto e : m_entities) {
            auto is_reliable = true;
            for (auto t : system.componentsUsed()) {
                if (!hasComponent(e, t)) {
                    is_reliable = false;
                    break;
                }
            }

            if (is_reliable) system.addEntity(e);
        }
    }
} // namespace stormkit::entities
