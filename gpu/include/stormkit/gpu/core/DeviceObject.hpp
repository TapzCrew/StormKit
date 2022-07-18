// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/gpu/core/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC DeviceObject {
      public:
        explicit DeviceObject(const Device &device) noexcept;
        ~DeviceObject() noexcept;

        DeviceObject(const DeviceObject &) noexcept;
        auto operator=(const DeviceObject &) noexcept -> DeviceObject &;

        DeviceObject(DeviceObject &&) noexcept;
        auto operator=(DeviceObject &&) noexcept -> DeviceObject &;

        [[nodiscard]] auto device() const noexcept -> const Device &;

      private:
        const Device *m_device;
    };
} // namespace stormkit::gpu

#include "DeviceObject.inl"
