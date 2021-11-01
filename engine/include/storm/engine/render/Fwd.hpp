// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/ResourcesPool.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/resource/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/render/3D/Fwd.hpp>
#include <storm/engine/render/framegraph/Fwd.hpp>
#include <storm/engine/render/utils/Fwd.hpp>

namespace storm::engine {
    class Camera;
    DECLARE_PTR_AND_REF(Camera);

    class Drawable;
    DECLARE_PTR_AND_REF(Drawable);

    struct DrawableComponent;

    class Material;
    DECLARE_PTR_AND_REF(Material);

    struct MaterialComponent;

    class RenderQueue;
    DECLARE_PTR_AND_REF(RenderQueue);

    class RenderSystem;
    DECLARE_PTR_AND_REF(RenderSystem);

    class Transform;
    DECLARE_PTR_AND_REF(Transform);

    struct TransformComponent;

    class VertexArray;
    DECLARE_PTR_AND_REF(VertexArray);

    using IndexArray     = std::vector<core::UInt16>;
    using IndexSpan      = core::span<core::UInt16>;
    using IndexConstSpan = core::span<const core::UInt16>;

    using LargeIndexArray     = std::vector<core::UInt32>;
    using LargeIndexSpan      = core::span<core::UInt32>;
    using LargeIndexConstSpan = core::span<const core::UInt32>;

    using IndexArrayProxy     = std::variant<IndexArray, LargeIndexArray>;
    using IndexSpanProxy      = std::variant<IndexSpan, LargeIndexSpan>;
    using IndexConstSpanProxy = std::variant<IndexConstSpan, LargeIndexConstSpan>;

    using ShaderPool  = core::ResourcesPool<std::string, render::Shader>;
    using TexturePool = core::ResourcesPool<std::string, render::Texture>;
} // namespace storm::engine
