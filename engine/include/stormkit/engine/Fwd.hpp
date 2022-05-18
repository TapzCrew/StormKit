// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
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

#include <stormkit/engine/inputs/Fwd.hpp>
#include <stormkit/engine/render/Fwd.hpp>

namespace stormkit::engine {
    class Engine;
    DECLARE_PTR_AND_REF(Engine);
} // namespace stormkit::engine
