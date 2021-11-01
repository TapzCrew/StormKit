// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/Fwd.hpp>
#include <storm/render/core/Surface.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC OffscreenSurface final: public Surface {
      public:
        OffscreenSurface(core::Extentu extent,
                         const Instance &instance,
                         Buffering buffering = Buffering::Triple);
        ~OffscreenSurface() override;

        OffscreenSurface(OffscreenSurface &&);
        OffscreenSurface &operator=(OffscreenSurface &&);

        void initialize(const Device &device) override;
        void recreate() override;
        void destroy() override;

        Frame acquireNextFrame() override;
        void present(const Frame &frame) override;
    };
} // namespace storm::render

//#include "OffscreenSurface.inl"
