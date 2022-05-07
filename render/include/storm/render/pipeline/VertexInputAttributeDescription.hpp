// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Types.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct VertexInputAttributeDescription {
        core::UInt32 location;
        core::UInt32 binding;
        Format format;
        core::Int32 offset;
    };

    using TaggedVertexInputAttributeDescriptionArray =
        storm::core::HashMap<std::string, VertexInputAttributeDescription>;
    DECLARE_ARRAYS(VertexInputAttributeDescription)
} // namespace storm::render

HASH_FUNC(storm::render::VertexInputAttributeDescription)
HASH_FUNC(storm::render::VertexInputAttributeDescriptionArray)
HASH_FUNC(storm::render::VertexInputAttributeDescriptionSpan)
HASH_FUNC(storm::render::VertexInputAttributeDescriptionConstSpan)
