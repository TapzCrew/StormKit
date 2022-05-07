// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Memory.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct VertexBindingDescription {
        core::UInt32 binding;
        core::ArraySize stride;
        VertexInputRate input_rate = VertexInputRate::Vertex;
    };

    DECLARE_ARRAYS(VertexBindingDescription)
} // namespace storm::render

HASH_FUNC(storm::render::VertexBindingDescription)
HASH_FUNC(storm::render::VertexBindingDescriptionArray)
HASH_FUNC(storm::render::VertexBindingDescriptionSpan)
HASH_FUNC(storm::render::VertexBindingDescriptionConstSpan)
