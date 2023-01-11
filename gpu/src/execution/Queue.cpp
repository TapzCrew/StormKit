// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(const Device& device, const Device::QueueEntry& entry)
        : DeviceObject { device }, m_entry { entry } {
        const auto& vk = this->device().table();

        vk.vkGetDeviceQueue(this->device(), m_entry.id, 0, &m_queue);

        core::ensures(m_queue != VK_NULL_HANDLE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::~Queue() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(Queue&& other) noexcept
        : DeviceObject { std::move(other) }, m_entry { std::move(other.m_entry) },
          m_queue { std::exchange(other.m_queue, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::operator=(Queue&& other) noexcept -> Queue& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_entry = std::move(other.m_entry);
        m_queue = std::exchange(other.m_queue, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::waitIdle() const noexcept -> void {
        core::expects(m_queue);

        const auto& vk = device().table();

        CHECK_VK_ERROR(vk.vkQueueWaitIdle(m_queue));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::submit(std::span<const CommandBuffer *> commandbuffers,
                       std::span<const Semaphore *> wait_semaphores,
                       std::span<const Semaphore *> signal_semaphores,
                       Fence *fence) const noexcept -> void {
        core::expects(m_queue);

        const auto& vk = device().table();

        const auto toVkHandle = [](const auto& elem) noexcept { return elem->vkHandle(); };

        const auto vk_commandbuffers    = core::transform(commandbuffers, toVkHandle);
        const auto vk_wait_semaphores   = core::transform(wait_semaphores, toVkHandle);
        const auto vk_signal_semaphores = core::transform(signal_semaphores, toVkHandle);

        auto wait_stages = std::vector<VkPipelineStageFlags> {};
        wait_stages.resize(core::as<core::RangeExtent>(std::size(wait_semaphores)),
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        const auto submit_info = VkSubmitInfo {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount   = core::as<core::UInt32>(std::size(vk_wait_semaphores)),
            .pWaitSemaphores      = std::data(vk_wait_semaphores),
            .pWaitDstStageMask    = std::data(wait_stages),
            .commandBufferCount   = core::as<core::UInt32>(std::size(vk_commandbuffers)),
            .pCommandBuffers      = std::data(vk_commandbuffers),
            .signalSemaphoreCount = core::as<core::UInt32>(std::size(vk_signal_semaphores)),
            .pSignalSemaphores    = std::data(vk_signal_semaphores)
        };

        auto vk_fence = VkFence { VK_NULL_HANDLE };
        if (fence) vk_fence = *fence;

        const auto submit_infos = std::array { submit_info };
        CHECK_VK_ERROR(vk.vkQueueSubmit(m_queue,
                                        core::as<core::UInt32>(std::size(submit_infos)),
                                        std::data(submit_infos),
                                        vk_fence));
    }
} // namespace stormkit::gpu
