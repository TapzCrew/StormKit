// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <memory>
#include <optional>
#include <queue>
#include <span>
#include <vector>

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::entities {
    struct Component;
    DECLARE_PTR_AND_REF(Component)

    class EntityManager;
    DECLARE_PTR_AND_REF(EntityManager)

    class System;
    DECLARE_PTR_AND_REF(System)

    class MessageBus;
    DECLARE_PTR_AND_REF(MessageBus)

    struct Message;
} // namespace stormkit::entities
