// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::render {
    class Fence;
    DECLARE_PTR_AND_REF(Fence);

    class Semaphore;
    DECLARE_PTR_AND_REF(Semaphore);
} // namespace stormkit::render
