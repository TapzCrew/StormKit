// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.entities.system;

// clang-format off
import stormkit.entities.entitymanager;
// clang-format on
#else
    /////////// - StormKit::log - ///////////
    #include <stormkit/entities/EntityManager.mpp>
    #include <stormkit/entities/System.mpp>
#endif

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    System::System(EntityManager &manager, core::UInt32 priority, ComponentTypes &&types)
        : m_manager { &manager }, m_priority { priority }, m_types { std::move(types) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    System::System(EntityManager &manager, core::UInt32 priority, const ComponentTypes &types)
        : m_manager { &manager }, m_priority { priority }, m_types { types } {}

    /////////////////////////////////////
    /////////////////////////////////////
    System::System(System &&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto System::operator=(System &&) -> System & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    System::~System() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto System::preUpdate() -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto System::postUpdate() -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto System::addEntity(Entity e) -> void {
        STORMKIT_EXPECTS(e != INVALID_ENTITY);

        m_entities.insert(e);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto System::removeEntity(Entity e) -> void {
        STORMKIT_EXPECTS(e != INVALID_ENTITY);

        m_entities.erase(e);
    }
} // namespace stormkit::entities
