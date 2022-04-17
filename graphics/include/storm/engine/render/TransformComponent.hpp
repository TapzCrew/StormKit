// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/Component.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Transform.hpp>

namespace storm::engine {
    struct STORMKIT_PUBLIC TransformComponent: public entities::Component {
        TransformComponent() noexcept = default;

        TransformComponent(TransformComponent &&) noexcept = default;
        TransformComponent &operator=(TransformComponent &&) noexcept = default;

        Transform transform;

        static constexpr auto TYPE = entities::componentHash("Transform");
    };
} // namespace storm::engine
