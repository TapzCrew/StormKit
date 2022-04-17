// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "EntityManager.mpp"

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    auto EntityManager::addComponent(Entity entity, Args &&...args) -> T & {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        STORMKIT_EXPECTS(hasEntity(entity));

        auto component = std::make_unique<T>(std::forward<Args>(args)...);

        auto type                  = T::TYPE;
        m_components[entity][type] = std::move(component);

        m_updated_entities.emplace(entity);

        return getComponent<T>(entity);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::destroyComponent(Entity entity) -> void {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        STORMKIT_EXPECTS(hasEntity(entity));
        STORMKIT_EXPECTS(hasComponent<T>(entity));

        auto it = std::find_if(std::begin(m_components[entity]),
                               std::end(m_components[entity]),
                               [](auto &i) {
                                   if (i.first == T::TYPE) return true;

                                   return false;
                               });

        m_components[entity].erase(it);

        m_updated_entities.emplace(entity);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::hasComponent(Entity entity) const -> bool {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        STORMKIT_EXPECTS(hasEntity(entity));

        auto it = std::find_if(std::begin(m_components.at(entity)),
                               std::end(m_components.at(entity)),
                               [](auto &i) {
                                   if (i.first == T::TYPE) return true;

                                   return false;
                               });

        return (it != std::end(m_components.at(entity)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::entitiesWithComponent() const -> std::vector<Entity> {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        auto entities = std::vector<Entity> {};

        std::for_each(std::cbegin(m_components),
                      std::cend(m_components),
                      [&entities](const auto &pair) {
                          const auto &map = pair.second;
                          auto it         = std::find_if(std::cbegin(map),
                                                 std::cend(map),
                                                 [](const auto &component) {
                                                     if (component.first == T::TYPE) return true;

                                                     return false;
                                                 });

                          if (it != std::cend(map)) entities.emplace_back(pair.first);
                      });

        return entities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::getComponent(Entity entity) -> T & {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        STORMKIT_EXPECTS(hasComponent<T>(entity));
        STORMKIT_EXPECTS(hasEntity(entity));

        auto type = T::TYPE;
        return *static_cast<T *>(m_components.at(entity).at(type).get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::getComponent(Entity entity) const -> const T & {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        STORMKIT_EXPECTS(hasComponent<T>(entity));
        STORMKIT_EXPECTS(hasEntity(entity));

        auto type = T::TYPE;
        return *static_cast<const T *>(m_components.at(entity).at(type).get());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::componentsOfType() -> std::vector<std::reference_wrapper<T>> {
        auto vec = std::vector<std::reference_wrapper<T>> {};

        for (auto i : m_entities) {
            if (hasComponent<T>(i)) { vec.emplace_back(getComponent<T>(i)); }
        }

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    auto EntityManager::addSystem(Args &&...args) -> T & {
        auto system = std::make_unique<T>(std::forward<Args &&>(args)..., *this);
        auto &ref   = static_cast<T &>(*system);

        getNeededEntities(ref);
        m_systems.emplace(std::move(system));

        return ref;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::hasSystem() const noexcept -> bool {
        static_assert(std::is_base_of<System, T>::value, "T must be a System");

        for (auto &system : m_systems) {
            if (auto system_ptr = dynamic_cast<const T *>(system.get()); system_ptr != nullptr)
                return true;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::getSystem() -> T & {
        using TPtr = T *;

        STORMKIT_EXPECTS(hasSystem<T>());

        auto system_observer = TPtr { nullptr };
        for (auto &system : m_systems) {
            if (auto system_ptr = dynamic_cast<T *>(system.get()); system_ptr != nullptr) {
                system_observer = system_ptr;
                break;
            }
        }

        return *system_observer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto EntityManager::getSystem() const -> const T & {
        using TConstPtr = const T *;

        STORMKIT_EXPECTS(hasSystem<const T>());

        auto system_observer = TConstPtr { nullptr };
        for (auto &system : m_systems) {
            if (auto system_ptr = dynamic_cast<const T *>(system.get()); system_ptr != nullptr) {
                system_observer = system_ptr;
                break;
            }
        }

        return *system_observer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityManager::numberOfEntities() const noexcept -> core::USize {
        return std::size(m_entities);
    }
} // namespace stormkit::entities
