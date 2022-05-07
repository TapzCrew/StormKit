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
    class FPSCamera;
    DECLARE_PTR_AND_REF(FPSCamera);

    class PbrMaterial;
    DECLARE_PTR_AND_REF(PbrMaterial);

    class PbrMesh;
    DECLARE_PTR_AND_REF(PbrMesh);

    class PbrRenderSystem;
    DECLARE_PTR_AND_REF(PbrRenderSystem);

    class Model;
    DECLARE_PTR_AND_REF(Model);
} // namespace storm::engine
