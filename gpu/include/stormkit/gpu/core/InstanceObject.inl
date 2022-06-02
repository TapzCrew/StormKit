// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "InstanceObject.mpp"

namespace stormkit::gpu {
    inline InstanceObject::InstanceObject(const Instance &instance) noexcept
        : m_instance { &instance } {}
    inline InstanceObject::~InstanceObject() noexcept = default;

    inline InstanceObject::InstanceObject(const InstanceObject &) noexcept = default;
    inline auto InstanceObject::operator=(const InstanceObject &) noexcept
        -> InstanceObject             & = default;

    inline InstanceObject::InstanceObject(InstanceObject &&) noexcept = default;
    inline auto InstanceObject::operator=(InstanceObject &&) noexcept -> InstanceObject & = default;

    inline auto InstanceObject::instance() const noexcept -> const Instance & {
        STORMKIT_EXPECTS(m_instance);

        return *m_instance;
    }
} // namespace stormkit::gpu
