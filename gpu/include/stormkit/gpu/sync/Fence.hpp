// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <chrono>
#include <optional>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Fence: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Fence;

        enum class Status { Signaled, Unsignaled };

        explicit Fence(const Device &device, bool signaled = false);
        ~Fence();

        Fence(const Fence &) = delete;
        auto operator=(const Fence &) -> Fence & = delete;

        Fence(Fence &&) noexcept;
        auto operator=(Fence &&) noexcept -> Fence &;

        auto wait(std::chrono::milliseconds wait_for = std::chrono::milliseconds::max()) const
            -> Result;
        auto reset() -> void;

        [[nodiscard]] auto status() const noexcept -> Status;

        [[nodiscard]] operator VkFence() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkFence;

      private:
        VkFence m_fence = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "Fence.inl"
