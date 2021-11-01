// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    T &EntityManager::addComponent(Entity entity, Args &&...args) {
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
    void EntityManager::destroyComponent(Entity entity) {
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
    bool EntityManager::hasComponent(Entity entity) const {
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
    std::vector<Entity> EntityManager::entitiesWithComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
        static_assert(T::TYPE != Component::INVALID_TYPE, "T must have T::type defined");

        auto entities = std::vector<Entity> {};

        std::for_each(std::begin(m_components),
                      std::end(m_components),
                      [&entities](const auto &pair) {
                          const auto &map = pair.second;
                          auto it         = std::find_if(std::begin(map),
                                                 std::end(map),
                                                 [](const auto &component) {
                                                     if (component.first == T::TYPE) return true;

                                                     return false;
                                                 });

                          if (it != std::end(map)) entities.push_back(pair.first);
                      });

        return entities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    T &EntityManager::getComponent(Entity entity) {
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
    const T &EntityManager::getComponent(Entity entity) const {
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
    std::vector<std::reference_wrapper<T>> EntityManager::componentsOfType() {
        auto vec = std::vector<std::reference_wrapper<T>> {};

        for (auto i : m_entities) {
            if (hasComponent<T>(i)) { vec.emplace_back(getComponent<T>(i)); }
        }

        return vec;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... Args>
    T &EntityManager::addSystem(Args &&...args) {
        auto system = std::make_unique<T>(std::forward<Args &&>(args)..., *this);
        auto &ref   = static_cast<T &>(*system);

        getNeededEntities(ref);
        m_systems.emplace(std::move(system));

        return ref;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    bool EntityManager::hasSystem() const noexcept {
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
    T &EntityManager::getSystem() {
        STORMKIT_EXPECTS(hasSystem<T>());

        auto system_observer = core::makeObserver<T>(nullptr);
        for (auto &system : m_systems) {
            if (auto system_ptr = dynamic_cast<T *>(system.get()); system_ptr != nullptr) {
                system_observer = core::makeObserver(system_ptr);
                break;
            }
        }

        return *system_observer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    const T &EntityManager::getSystem() const {
        STORMKIT_EXPECTS(hasSystem<const T>());

        auto system_observer = core::makeConstObserver<T>(nullptr);
        for (auto &system : m_systems) {
            if (auto system_ptr = dynamic_cast<const T *>(system.get()); system_ptr != nullptr) {
                system_observer = core::makeConstObserver(system_ptr);
                break;
            }
        }

        return *system_observer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::ArraySize EntityManager::numberOfEntities() const noexcept {
        return std::size(m_entities);
    }
} // namespace storm::entities
