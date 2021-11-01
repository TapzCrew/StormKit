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

#include <storm/engine/render/Material.hpp>

namespace storm::engine {
    struct STORMKIT_PUBLIC MaterialComponent: public entities::Component {
        MaterialComponent() noexcept = default;

        MaterialComponent(MaterialComponent &&) noexcept = default;
        MaterialComponent &operator=(MaterialComponent &&) noexcept = default;

        MaterialOwnedPtr drawable;

        static constexpr auto TYPE = entities::componentHash("Material");
    };
} // namespace storm::engine
