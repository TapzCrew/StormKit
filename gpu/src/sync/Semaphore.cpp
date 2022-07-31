// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.hpp>

#include <stormkit/gpu/sync/Semaphore.hpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(const Device& device) : DeviceObject { device } {
        const auto& vk = this->device().table();

        const auto create_info =
            VkSemaphoreCreateInfo { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = {} };

        CHECK_VK_ERROR(vk.vkCreateSemaphore(this->device(), &create_info, nullptr, &m_semaphore));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::~Semaphore() {
        if (m_semaphore != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroySemaphore(device(), m_semaphore, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(Semaphore&& other) noexcept
        : DeviceObject { std::move(other) }, m_semaphore { std::exchange(other.m_semaphore,
                                                                         VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Semaphore::operator=(Semaphore&& other) noexcept -> Semaphore& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_semaphore = std::exchange(other.m_semaphore, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
