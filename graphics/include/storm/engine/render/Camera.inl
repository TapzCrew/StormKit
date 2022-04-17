// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::type() const noexcept -> Camera::Type { return m_type; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::setViewport(core::Extentf viewport) noexcept -> void {
        m_viewport = std::move(viewport);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::viewport() const noexcept -> const core::Extentf & { return m_viewport; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::setPosition(core::Vector3f position) noexcept -> void {
        m_position = std::move(position);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::position() const noexcept -> const core::Vector3f & { return m_position; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::setRotation(core::Vector3f orientation) noexcept -> void {
        m_orientation = std::move(orientation);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::rotation() const noexcept -> const core::Vector3f & {
        return m_orientation;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::setMoveSpeed(core::Vector3f move_speed) noexcept -> void {
        m_move_speed = std::move(move_speed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::moveSpeed() const noexcept -> const core::Vector3f & {
        return m_move_speed;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::setRotationSpeed(core::Vector3f rotation_speed) noexcept -> void {
        m_rotation_speed = std::move(rotation_speed);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::rotationSpeed() const noexcept -> const core::Vector3f & {
        return m_rotation_speed;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::dirty() const noexcept -> bool { return m_dirty; }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Camera::data() const noexcept -> const Data & {
        m_dirty = false;

        return m_data;
    }
} // namespace storm::engine
