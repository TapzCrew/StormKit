// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.hpp>

#include <stormkit/gpu/sync/Fence.hpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(const Device &device, bool signaled) : DeviceObject { device } {
        const auto &vk = this->device().table();

        const auto flags = (signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags {};

        const auto create_info =
            VkFenceCreateInfo { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = flags };

        CHECK_VK_ERROR(vk.vkCreateFence(this->device(), &create_info, nullptr, &m_fence));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::~Fence() {
        if (m_fence != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyFence(device(), m_fence, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(Fence &&other) noexcept
        : DeviceObject { std::move(other) }, m_fence { std::exchange(other.m_fence,
                                                                     VK_NULL_HANDLE) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::operator=(Fence &&other) noexcept -> Fence & {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_fence               = std::exchange(other.m_fence, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::wait(std::chrono::milliseconds wait_for) const -> Result {
        STORMKIT_EXPECTS(m_fence != VK_NULL_HANDLE);

        const auto &vk = device().table();
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
        const auto &vk = device().table();

        CHECK_VK_ERROR(vk.vkResetFences(device(), 1, &m_fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::status() const noexcept -> Status {
        STORMKIT_EXPECTS(m_fence != VK_NULL_HANDLE);
        const auto &vk = device().table();

        const auto status = vk.vkGetFenceStatus(device(), m_fence);
        if (status == VK_SUCCESS) return Status::Signaled;
        else if (status == VK_NOT_READY)
            return Status::Unsignaled;

        CHECK_VK_ERROR(status)

        return Status::Unsignaled; // can't go here
    }
} // namespace stormkit::gpu
