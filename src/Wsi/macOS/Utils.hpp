// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution



/////////// - Foundation - ///////////
#import <Foundation/NSGeometry.h>

/////////// - StormKit::core - ///////////
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>
#include <storm/window/VideoSettings.hpp>

namespace storm::window::details {
    STORMKIT_PRIVATE [[nodiscard]] MouseButton toStormMouseButton(int button) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] Key localizedKeyToStormKey(char code) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] Key nonLocalizedKeytoStormKey(unsigned short code) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] core::math::Vector2U toStormVec(NSPoint point) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] core::Extentu toStormVec(CGSize size) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] NSPoint fromStormVec(const core::math::Vector2U &point) noexcept;
    STORMKIT_PRIVATE [[nodiscard]] core::UInt8 usageToVirtualCode(core::UInt32 usage) noexcept;
} // namespace storm::window::details
