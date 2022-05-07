// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::position() const noexcept -> const core::Vector3f & {
        return m_position;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setPosition(const core::Vector2f &position) noexcept -> void {
        setXPosition(position.x);
        setYPosition(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setPosition(const core::Vector3f &position) noexcept -> void {
        setXPosition(position.x);
        setYPosition(position.y);
        setZPosition(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setPosition(float x, float y, float z) noexcept -> void {
        setXPosition(x);
        setYPosition(y);
        setZPosition(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setXPosition(float x) noexcept -> void {
        m_position.x = x;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setYPosition(float y) noexcept -> void {
        m_position.y = y;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setZPosition(float z) noexcept -> void {
        m_position.z = z;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::move(const core::Vector2f &position) noexcept -> void {
        moveX(position.x);
        moveY(position.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::move(const core::Vector3f &position) noexcept -> void {
        moveX(position.x);
        moveY(position.y);
        moveZ(position.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::move(float x, float y, float z) noexcept -> void {
        moveX(x);
        moveY(y);
        moveZ(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::moveX(float x) noexcept -> void {
        m_position.x += x;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::moveY(float y) noexcept -> void {
        m_position.y += y;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::moveZ(float z) noexcept -> void {
        m_position.z += z;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scale() const noexcept -> const core::Vector3f & { return m_scale; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setScale(const core::Vector2f &scale) noexcept -> void {
        setXScale(scale.x);
        setYScale(scale.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setScale(const core::Vector3f &scale) noexcept -> void {
        setXScale(scale.x);
        setYScale(scale.y);
        setZScale(scale.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setScale(float x, float y, float z) noexcept -> void {
        setXScale(x);
        setYScale(y);
        setZScale(z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setXScale(float x) noexcept -> void {
        m_scale.x = x;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setYScale(float y) noexcept -> void {
        m_scale.y = y;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setZScale(float z) noexcept -> void {
        m_scale.z = z;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scale(const core::Vector3f &scale) noexcept -> void {
        scaleX(scale.x);
        scaleY(scale.y);
        scaleZ(scale.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scale(const core::Vector2f &scale) noexcept -> void {
        scaleX(scale.x);
        scaleY(scale.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scale(float x_scale, float y_scale, float z_scale) noexcept -> void {
        scaleX(x_scale);
        scaleY(y_scale);
        scaleZ(z_scale);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scaleX(float scale) noexcept -> void {
        m_scale.x += scale;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scaleY(float scale) noexcept -> void {
        m_scale.y += scale;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::scaleZ(float scale) noexcept -> void {
        m_scale.z += scale;

        m_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::orientation() const noexcept -> const core::Quaternion & {
        if (m_orientation_dirty) {
            recomputeOrientation();
            m_orientation_dirty = false;
        }

        return m_orientation;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::orientationEuler() const noexcept -> const core::Vector3f & {
        return m_orientation_euler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setOrientation(const core::Vector3f &orientation) noexcept -> void {
        setPitch(orientation.x);
        setYaw(orientation.y);
        setRoll(orientation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setOrientation(const core::Vector2f &orientation) noexcept -> void {
        setPitch(orientation.x);
        setYaw(orientation.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setOrientation(float pitch, float yaw, float roll) noexcept -> void {
        setPitch(pitch);
        setYaw(yaw);
        setRoll(roll);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setYaw(float yaw) noexcept -> void {
        m_orientation_euler.y = yaw;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setPitch(float pitch) noexcept -> void {
        m_orientation_euler.x = pitch;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setRoll(float roll) noexcept -> void {
        m_orientation_euler.z = roll;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotate(const core::Vector3f &rotation) noexcept -> void {
        rotatePitch(rotation.x);
        rotateYaw(rotation.y);
        rotateRoll(rotation.z);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotate(const core::Vector2f &rotation) noexcept -> void {
        rotatePitch(rotation.x);
        rotateYaw(rotation.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotate(float pitch, float yaw, float roll) noexcept -> void {
        rotatePitch(pitch);
        rotateYaw(yaw);
        rotateRoll(roll);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotateYaw(float yaw) noexcept -> void {
        m_orientation_euler.y += yaw;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotatePitch(float pitch) noexcept -> void {
        m_orientation_euler.x += pitch;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::rotateRoll(float roll) noexcept -> void {
        m_orientation_euler.z += roll;

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::setMatrix(const core::Matrix &matrix) noexcept -> void {
        extract(matrix);

        m_dirty             = true;
        m_orientation_dirty = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::data() const noexcept -> const Data & {
        if (m_dirty) {
            recomputeMatrices();
            m_dirty = false;
        }

        return m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Transform::dirty() const noexcept -> bool { return m_dirty; }
} // namespace storm::engine
