
// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>

namespace stormkit::gpu {
    struct AccelerationStructureDescription;
    class STORMKIT_PUBLIC AccelerationStructure: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Render_Pass;

        AccelerationStructure(const Device &device,
                              const AccelerationStructureDescription &description);
        ~AccelerationStructure();

        AccelerationStructure(const AccelerationStructure &) = delete;
        auto operator=(const AccelerationStructure &) -> AccelerationStructure & = delete;

        AccelerationStructure(AccelerationStructure &&) noexcept;
        auto operator=(AccelerationStructure &&) noexcept -> AccelerationStructure &;

        [[nodiscard]] operator VkAccelerationStructureKHR() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkAccelerationStructure;

      private:
        VkAccelerationStructureKHR m_render_pass = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "AccelerationStructure.inl"