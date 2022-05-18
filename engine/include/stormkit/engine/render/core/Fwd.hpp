// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>


#include <stormkit/core/MemoryMacro.hpp>

namespace stormkit::engine {
    class RenderQueue;
    DECLARE_PTR_AND_REF(RenderQueue);

    class VertexArray;
    DECLARE_PTR_AND_REF(VertexArray);
} // namespace stormkit::engine
