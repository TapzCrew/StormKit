// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Semaphore: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Semaphore;

        explicit Semaphore(const Device &device);
        ~Semaphore();

        Semaphore(const Semaphore &) = delete;
        auto operator=(const Semaphore &) -> Semaphore & = delete;

        Semaphore(Semaphore &&) noexcept;
        auto operator=(Semaphore &&) noexcept -> Semaphore &;

        [[nodiscard]] operator VkSemaphore() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkSemaphore;

      private:
        VkSemaphore m_semaphore = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "Semaphore.inl"
