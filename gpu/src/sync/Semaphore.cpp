// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.mpp>

#include <stormkit/gpu/sync/Semaphore.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(const Device &device) : m_device { &device } {
        const auto &vk = m_device->table();

        const auto create_info = VkSemaphoreCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        CHECK_VK_ERROR(vk.vkCreateSemaphore(*m_device, &create_info, nullptr, &m_semaphore));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::~Semaphore() {
        if (m_semaphore != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = m_device->table();

            vk.vkDestroySemaphore(*m_device, m_semaphore, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(Semaphore &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) }, m_semaphore {
              std::exchange(other.m_semaphore, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Semaphore::operator=(Semaphore &&other) noexcept -> Semaphore & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device    = std::exchange(other.m_device, nullptr);
        m_semaphore = std::exchange(other.m_semaphore, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
