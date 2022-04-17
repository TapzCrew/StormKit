// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Camera.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC FPSCamera final: public Camera {
      public:
        struct Inputs {
            bool up    = false;
            bool down  = false;
            bool right = false;
            bool left  = false;

            bool mouse_updated = false;
            bool mouse_ignore  = false;
            float x_mouse      = 0.f;
            float y_mouse      = 0.f;
        };

        FPSCamera(core::Extentf viewport, float znear = 0.1f, float zfar = 100.f) noexcept;
        ~FPSCamera() noexcept override;

        FPSCamera(FPSCamera &&) noexcept;
        FPSCamera &operator=(FPSCamera &&) noexcept;

        void feedInputs(Inputs inputs);
        void update(core::Secondf delta) noexcept override;

        ALLOCATE_HELPERS(FPSCamera)
      private:
        Inputs m_inputs;

        core::Vector3f m_front               = { 0.f, 0.f, 0.f };
        static constexpr core::Vector3f m_up = { 0.f, 1.f, 0.f };

        float m_last_x_mouse = 0.f;
        float m_last_y_mouse = 0.f;
    };
} // namespace storm::engine

#include "FPSCamera.inl"
