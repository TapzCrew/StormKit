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
    class STORMKIT_PUBLIC Transform {
      public:
        struct alignas(16) Data {
            core::Matrix transform     = core::Matrix { 1.f };
            core::Matrix inv_transform = core::inverse(core::Matrix { 1.f });
        };

        Transform() noexcept;
        ~Transform() noexcept;

        Transform(const Transform &) noexcept;
        Transform &operator=(const Transform &) noexcept;

        Transform(Transform &&) noexcept;
        Transform &operator=(Transform &&) noexcept;

        [[nodiscard]] const core::Vector3f &position() const noexcept;

        void setPosition(const core::Vector2f &Int32) noexcept;
        void setPosition(const core::Vector3f &Int32) noexcept;
        void setPosition(float x, float y, float z = 0.f) noexcept;
        void setXPosition(float x) noexcept;
        void setYPosition(float y) noexcept;
        void setZPosition(float z) noexcept;

        void move(const core::Vector2f &Int32) noexcept;
        void move(const core::Vector3f &Int32) noexcept;
        void move(float x, float y, float z = 0.f) noexcept;
        void moveX(float x) noexcept;
        void moveY(float y) noexcept;
        void moveZ(float z) noexcept;

        [[nodiscard]] const core::Vector3f &scale() const noexcept;

        void setScale(const core::Vector3f &scale) noexcept;
        void setScale(const core::Vector2f &scale) noexcept;
        void setScale(float x_scale, float y_scale, float z_scale = 1.f) noexcept;
        void setXScale(float scale) noexcept;
        void setYScale(float scale) noexcept;
        void setZScale(float scale) noexcept;

        void scale(const core::Vector3f &scale) noexcept;
        void scale(const core::Vector2f &scale) noexcept;
        void scale(float x_scale, float y_scale, float z_scale = 0.f) noexcept;
        void scaleX(float scale) noexcept;
        void scaleY(float scale) noexcept;
        void scaleZ(float scale) noexcept;

        [[nodiscard]] const core::Quaternion &orientation() const noexcept;
        [[nodiscard]] const core::Vector3f &orientationEuler() const noexcept;

        void setOrientation(const core::Vector3f &orientation) noexcept;
        void setOrientation(const core::Vector2f &orientation) noexcept;
        void setOrientation(float pitch, float yaw, float roll = 0.f) noexcept;
        void setYaw(float pitch) noexcept;
        void setPitch(float yaw) noexcept;
        void setRoll(float roll) noexcept;

        void rotate(const core::Vector3f &rotation) noexcept;
        void rotate(const core::Vector2f &rotation) noexcept;
        void rotate(float yaw, float pitch, float roll = 0.f) noexcept;
        void rotateYaw(float value) noexcept;
        void rotatePitch(float value) noexcept;
        void rotateRoll(float value) noexcept;

        void setMatrix(const core::Matrix &matrix) noexcept;
        [[nodiscard]] const Data &data() const noexcept;

        [[nodiscard]] bool dirty() const noexcept;

      private:
        void recomputeOrientation() const noexcept;
        void recomputeMatrices() const noexcept;
        void extract(const core::Matrix &matrix) noexcept;

        core::Vector3f m_position          = core::Vector3f { 0.f, 0.f, 0.f };
        core::Vector3f m_orientation_euler = core::Vector3f { 0.f, 0.f, 0.f };
        core::Vector3f m_scale             = core::Vector3f { 1.f, 1.f, 1.f };

        mutable bool m_orientation_dirty = true;
        mutable bool m_dirty             = true;

        mutable core::Quaternion m_orientation = core::Quaternion { 0.f, 0.f, 0.f, 0.f };
        mutable Data m_data;
    };
} // namespace storm::engine

#include "Transform.inl"
