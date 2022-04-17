// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

namespace storm::engine {
    struct BoundingBox {
        core::Vector3f min;
        core::Vector3f max;
        core::Extentf extent;
    };
} // namespace storm::engine
