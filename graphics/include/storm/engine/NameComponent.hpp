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

namespace storm::engine {
    struct STORMKIT_PUBLIC NameComponent: public entities::Component {
        NameComponent() noexcept = default;

        NameComponent(const NameComponent &) = default;
        NameComponent &operator=(const NameComponent &) = default;

        NameComponent(NameComponent &&) noexcept = default;
        NameComponent &operator=(NameComponent &&) noexcept = default;

        std::string name;

        static constexpr auto TYPE = entities::componentHash("Name");
    };
} // namespace storm::engine
