// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <limits>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Fence: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Fence;

        enum class Status { Signaled, Unsignaled };

        explicit Fence(const Device &device);
        ~Fence();

        Fence(Fence &&);
        Fence &operator=(Fence &&);

        WaitResult wait(core::UInt64 wait_for = std::numeric_limits<core::UInt64>::max()) const;
        void reset();

        Status status() const noexcept;

        inline vk::Fence vkFence() const noexcept;
        inline operator vk::Fence() const noexcept;
        inline vk::Fence vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstPtr m_device;

        RAIIVkFence m_vk_fence;
    };
} // namespace storm::render

#include "Fence.inl"
