// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Semaphore: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Semaphore;

        explicit Semaphore(const Device &device);
        ~Semaphore();

        Semaphore(Semaphore &&);
        Semaphore &operator=(Semaphore &&);

        inline vk::Semaphore vkSemaphore() const noexcept;
        inline operator vk::Semaphore() const noexcept;
        inline vk::Semaphore vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstPtr m_device;

        RAIIVkSemaphore m_vk_semaphore;
    };
} // namespace storm::render

#include "Semaphore.inl"
