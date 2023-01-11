// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Core;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(const Device& device, bool signaled) : DeviceObject { device } {
        const auto& vk = this->device().table();

        const auto flags = (signaled) ? core::as<VkFenceCreateFlags>(VK_FENCE_CREATE_SIGNALED_BIT)
                                      : VkFenceCreateFlags {};

        const auto create_info = VkFenceCreateInfo { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                                     .pNext = nullptr,
                                                     .flags = flags };

        CHECK_VK_ERROR(vk.vkCreateFence(this->device(), &create_info, nullptr, &m_fence));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::~Fence() {
        if (m_fence != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyFence(device(), m_fence, nullptr);

            m_fence = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(Fence&& other) noexcept
        : DeviceObject { std::move(other) },
          m_fence { std::exchange(other.m_fence, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::operator=(Fence&& other) noexcept -> Fence& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_fence = std::exchange(other.m_fence, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::wait(std::chrono::milliseconds wait_for) const -> Result {
        core::expects(m_fence != VK_NULL_HANDLE);

        const auto& vk = device().table();
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
        core::expects(m_fence != VK_NULL_HANDLE);
        const auto& vk = device().table();

        CHECK_VK_ERROR(vk.vkResetFences(device(), 1, &m_fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::status() const noexcept -> Status {
        core::expects(m_fence != VK_NULL_HANDLE);
        const auto& vk = device().table();

        const auto status = vk.vkGetFenceStatus(device(), m_fence);
        if (status == VK_SUCCESS) return Status::Signaled;
        else if (status == VK_NOT_READY)
            return Status::Unsignaled;

        CHECK_VK_ERROR(status)

        return Status::Unsignaled; // can't go here
    }
} // namespace stormkit::gpu
