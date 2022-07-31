// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Surface.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC OffscreenSurface final: public Surface {
      public:
        OffscreenSurface(const core::ExtentU &extent,
                         const Instance &instance,
                         Buffering buffering = Buffering::Triple);
        ~OffscreenSurface() override;

        OffscreenSurface(const OffscreenSurface &) = delete;
        auto operator=(const OffscreenSurface &) -> OffscreenSurface & = delete;

        OffscreenSurface(OffscreenSurface &&) noexcept;
        auto operator=(OffscreenSurface &&) noexcept -> OffscreenSurface &;

        auto initialize(const Device &device) -> void override;
        auto recreate() -> void override;
        auto destroy() -> void override;

        auto acquireNextFrame() -> Expected<Frame> override;
        auto present(const Frame &frame) -> void override;
    };
} // namespace stormkit::gpu

//#include "OffscreenSurface.inl"