// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::engine {
    class RingHardwareBuffer;
    DECLARE_PTR_AND_REF(RingHardwareBuffer);

    class ResourceCache;
    DECLARE_PTR_AND_REF(ResourceCache);

    class DeletionQueue;
    DECLARE_PTR_AND_REF(DeletionQueue)
} // namespace storm::engine
