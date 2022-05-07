// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Types.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Camera {
      public:
        enum class Type { Perspective, Orthographic };

        struct alignas(16) Data {
            core::Vector4f position = core::Vector4f { 0.f, 0.f, 0.f, 0.f };
            core::Matrix projection = core::Matrix { 1.f };
            core::Matrix view       = core::Matrix { 1.f };
        };

        Camera(Type type, core::Extentf viewport, float znear = 0.1f, float zfar = 100.f) noexcept;
        virtual ~Camera() noexcept;

        Camera(const Camera &) noexcept;
        Camera &operator=(const Camera &) noexcept;

        Camera(Camera &&) noexcept;
        Camera &operator=(Camera &&) noexcept;

        [[nodiscard]] Type type() const noexcept;

        void setViewport(core::Extentf viewport) noexcept;
        [[nodiscard]] const core::Extentf &viewport() const noexcept;

        void setPosition(core::Vector3f position) noexcept;
        [[nodiscard]] const core::Vector3f &position() const noexcept;

        void setRotation(core::Vector3f orientation) noexcept;
        [[nodiscard]] const core::Vector3f &rotation() const noexcept;

        void setMoveSpeed(core::Vector3f move_speed) noexcept;
        [[nodiscard]] const core::Vector3f &moveSpeed() const noexcept;

        void setRotationSpeed(core::Vector3f rotation_speed) noexcept;
        [[nodiscard]] const core::Vector3f &rotationSpeed() const noexcept;

        virtual void update(core::Secondf delta) noexcept;

        [[nodiscard]] bool dirty() const noexcept;
        [[nodiscard]] const Data &data() const noexcept;

        ALLOCATE_HELPERS(Camera)
      protected:
        Type m_type;

        float m_near_plane = 0.1f;
        float m_far_plane  = 1000.f;
        float m_fov        = core::radians(45.f);

        core::Extentf m_viewport = { 0.f, 0.f };

        core::Vector3f m_orientation = { 0.f, 0.f, 0.f };
        core::Vector3f m_position    = { 0.f, 0.f, 0.f };

        core::Vector3f m_move_speed     = { 2.f, 2.f, 2.f };
        core::Vector3f m_rotation_speed = { 32.f, 32.f, 32.f };

        mutable bool m_dirty = true;
        Data m_data;
    };
} // namespace storm::engine

#include "Camera.inl"
