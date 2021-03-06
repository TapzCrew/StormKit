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

#include <stormkit/engine/render/core/Fwd.hpp>
#include <stormkit/engine/render/framegraphs/Fwd.hpp>

namespace stormkit::engine {
    struct MeshComponent;

    class Renderer;
    DECLARE_PTR_AND_REF(Renderer);
} // namespace stormkit::engine
