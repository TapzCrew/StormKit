// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/render/core/Device.mpp>

#include <stormkit/render/sync/Fence.mpp>

namespace stormkit::render {

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(const Device &device) : m_device { &device } {
        const auto &vk = m_device->table();

        const auto create_info = VkFenceCreateInfo { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                     .flags = VK_FENCE_CREATE_SIGNALED_BIT };

        CHECK_VK_ERROR(vk.vkCreateFence(*m_device, &create_info, nullptr, &m_fence));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::~Fence() {
        if (m_fence != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = m_device->table();

            vk.vkDestroyFence(device(), m_fence, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(Fence &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) }, m_fence {
              std::exchange(other.m_fence, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::operator=(Fence &&other) noexcept -> Fence & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device = std::exchange(other.m_device, nullptr);
        m_fence  = std::exchange(other.m_fence, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::wait(std::chrono::milliseconds wait_for) const -> Result {
        STORMKIT_EXPECTS(m_fence != VK_NULL_HANDLE);

        const auto &vk = m_device->table();
        const auto result =
            vk.vkWaitForFences(device(),
                               1,
                               &m_fence,
                               VK_TRUE,
                               std::chrono::duration_cast<std::chrono::nanoseconds>(wait_for)
                                   .count());

        return core::as<Result>(result);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::reset() -> void {
        STORMKIT_EXPECTS(m_fence != VK_NULL_HANDLE);
        const auto &vk = m_device->table();

        CHECK_VK_ERROR(vk.vkResetFences(device(), 1, &m_fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::status() const noexcept -> Status {
        STORMKIT_EXPECTS(m_fence != VK_NULL_HANDLE);
        const auto &vk = m_device->table();

        const auto status = vk.vkGetFenceStatus(device(), m_fence);
        if (status == VK_SUCCESS) return Status::Signaled;
        else if (status == VK_NOT_READY)
            return Status::Unsignaled;

        CHECK_VK_ERROR(status)

        return Status::Unsignaled; // can't go here
    }
} // namespace stormkit::render
