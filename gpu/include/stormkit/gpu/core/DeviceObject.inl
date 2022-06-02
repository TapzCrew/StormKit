// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "DeviceObject.mpp"

namespace stormkit::gpu {
    inline DeviceObject::DeviceObject(const Device &device) noexcept : m_device { &device } {}
    inline DeviceObject::~DeviceObject() noexcept = default;

    inline DeviceObject::DeviceObject(const DeviceObject &) noexcept = default;
    inline auto DeviceObject::operator=(const DeviceObject &) noexcept -> DeviceObject & = default;

    inline DeviceObject::DeviceObject(DeviceObject &&) noexcept = default;
    inline auto DeviceObject::operator=(DeviceObject &&) noexcept -> DeviceObject & = default;

    inline auto DeviceObject::device() const noexcept -> const Device & {
        STORMKIT_EXPECTS(m_device);

        return *m_device;
    }
} // namespace stormkit::gpu
