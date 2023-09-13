// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Execution.CommandBuffer;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(const Device& device, const Device::QueueEntry& entry)
        : DeviceObject { device }, m_entry { entry } {
        try {
            m_vk_queue = this->device().vkHandle().getQueue(m_entry.id, 0);
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::~Queue() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(Queue&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::operator=(Queue&& other) noexcept -> Queue& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::submit(std::span<core::NakedRef<const CommandBuffer>> commandbuffers,
                       std::span<core::NakedRef<const Semaphore>> wait_semaphores,
                       std::span<core::NakedRef<const Semaphore>> signal_semaphores,
                       const Fence *fence) const noexcept -> void {
        const auto toVkHandle = [](const auto& elem) noexcept { return *elem->vkHandle(); };

        const auto vk_commandbuffers    = core::transform(commandbuffers, toVkHandle);
        const auto vk_wait_semaphores   = core::transform(wait_semaphores, toVkHandle);
        const auto vk_signal_semaphores = core::transform(signal_semaphores, toVkHandle);

        const auto wait_stages = std::vector<vk::PipelineStageFlags> {
            std::size(wait_semaphores),
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        const auto submit_info = vk::SubmitInfo {}
                                     .setWaitSemaphores(vk_wait_semaphores)
                                     .setWaitDstStageMask(wait_stages)
                                     .setCommandBuffers(vk_commandbuffers)
                                     .setSignalSemaphores(vk_signal_semaphores);

        const auto submit_infos = std::array { submit_info };
        if (fence) m_vk_queue->submit(submit_infos, *(fence->vkHandle()));
        else
            m_vk_queue->submit(submit_infos);
    }
} // namespace stormkit::gpu
