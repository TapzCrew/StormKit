// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <stormkit/core/Math.mpp>


#include <stormkit/entities/Component.mpp>


#include <stormkit/engine/render/core/VertexArray.mpp>

namespace stormkit::engine {
    using IndexArray = std::vector<core::UInt32>;

    struct SubMesh {
        VertexArray vertices;
        IndexArray indices;
    };

    struct MeshComponent: public stormkit::entities::Component {
        std::vector<SubMesh> submeshes;

        static constexpr Type TYPE = stormkit::entities::componentHash("MeshComponent");
    };
} // namespace stormkit::engine
