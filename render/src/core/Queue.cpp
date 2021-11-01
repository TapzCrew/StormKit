
// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <array>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Queue::Queue(const render::Device &device,
             render::QueueFlag flags,
             core::UInt32 family_index,
             vk::Queue queue)
    : m_device { &device }, m_queue_flag { flags }, m_family_index { family_index }, m_vk_queue {
          queue
      } {
    const auto create_info =
        vk::CommandPoolCreateInfo {}.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer |
                                              vk::CommandPoolCreateFlagBits::eTransient);

    m_vk_command_pool = m_device->createVkCommandPool(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Queue::~Queue() = default;

/////////////////////////////////////
/////////////////////////////////////
Queue::Queue(Queue &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Queue &Queue::operator=(Queue &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Queue::waitIdle() const noexcept {
    STORMKIT_EXPECTS(m_vk_queue);

    const auto result = m_vk_queue.waitIdle(m_device->vkDispatcher());
    STORMKIT_ENSURES(result == vk::Result::eSuccess);
}

/////////////////////////////////////
/////////////////////////////////////
void Queue::submit(core::span<const CommandBufferConstPtr> command_buffers,
                   core::span<const SemaphoreConstPtr> wait_semaphores,
                   core::span<const SemaphoreConstPtr> signal_semaphores,
                   FencePtr fence) const noexcept {
    auto vk_command_buffers = std::vector<vk::CommandBuffer> {};
    vk_command_buffers.reserve(gsl::narrow_cast<core::ArraySize>(
        std::size(command_buffers))); // remove narrow_cast when C++20 is avalaible on all platform
    auto vk_wait_semaphores = std::vector<vk::Semaphore> {};
    vk_wait_semaphores.reserve(gsl::narrow_cast<core::ArraySize>(std::size(wait_semaphores)));
    auto vk_signal_semaphores = std::vector<vk::Semaphore> {};
    vk_signal_semaphores.reserve(gsl::narrow_cast<core::ArraySize>(std::size(signal_semaphores)));

    for (const auto &command_buffer : command_buffers)
        vk_command_buffers.emplace_back(static_cast<const CommandBuffer &>(*command_buffer));

    for (const auto &semaphore : wait_semaphores) {
        if (!semaphore) continue;
        vk_wait_semaphores.emplace_back(static_cast<const Semaphore &>(*semaphore));
    }

    for (const auto &semaphore : signal_semaphores) {
        if (!semaphore) continue;
        vk_signal_semaphores.emplace_back(static_cast<const Semaphore &>(*semaphore));
    }

    auto wait_stages = std::vector<vk::PipelineStageFlags> {};
    wait_stages.resize(gsl::narrow_cast<core::ArraySize>(std::size(wait_semaphores)),
                       vk::PipelineStageFlagBits::eColorAttachmentOutput);

    const auto submit_info =
        vk::SubmitInfo {}
            .setWaitSemaphoreCount(gsl::narrow_cast<core::UInt32>(std::size(vk_wait_semaphores)))
            .setPWaitSemaphores(std::data(vk_wait_semaphores))
            .setPWaitDstStageMask(std::data(wait_stages))
            .setCommandBufferCount(gsl::narrow_cast<core::UInt32>(std::size(vk_command_buffers)))
            .setPCommandBuffers(std::data(vk_command_buffers))
            .setSignalSemaphoreCount(
                gsl::narrow_cast<core::UInt32>(std::size(vk_signal_semaphores)))
            .setPSignalSemaphores(std::data(vk_signal_semaphores));

    auto vk_fence = vk::Fence { VK_NULL_HANDLE };
    if (fence) {
        fence->reset();
        vk_fence = *fence;
    }

    const auto submit_infos = std::array { submit_info };
    const auto result       = m_vk_queue.submit(submit_infos, vk_fence, m_device->vkDispatcher());
    STORMKIT_ENSURES(result == vk::Result::eSuccess);
}

/////////////////////////////////////
/////////////////////////////////////
CommandBuffer Queue::createCommandBuffer(CommandBufferLevel level) const {
    auto command_buffer = std::move(createCommandBuffers(1, level)[0]);

    return command_buffer;
}

/////////////////////////////////////
/////////////////////////////////////
CommandBufferOwnedPtr Queue::createCommandBufferPtr(CommandBufferLevel level) const {
    auto command_buffer = std::move(createCommandBufferPtrs(1, level)[0]);

    return command_buffer;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::CommandBuffer>
    Queue::createCommandBuffers(core::ArraySize count, render::CommandBufferLevel level) const {
    const auto allocate_info = vk::CommandBufferAllocateInfo {}
                                   .setCommandPool(*m_vk_command_pool)
                                   .setLevel(toVK(level))
                                   .setCommandBufferCount(gsl::narrow_cast<core::UInt32>(count));

    auto vk_command_buffers = m_device->allocateVkCommandBuffers(allocate_info);

    auto command_buffers = std::vector<CommandBuffer> {};
    command_buffers.reserve(std::size(vk_command_buffers));

    for (auto &vk_command_buffer : vk_command_buffers)
        command_buffers.emplace_back(*this, level, std::move(vk_command_buffer));

    return command_buffers;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::CommandBufferOwnedPtr>
    Queue::createCommandBufferPtrs(core::ArraySize count, render::CommandBufferLevel level) const {
    const auto allocate_info = vk::CommandBufferAllocateInfo {}
                                   .setCommandPool(*m_vk_command_pool)
                                   .setLevel(toVK(level))
                                   .setCommandBufferCount(gsl::narrow_cast<core::UInt32>(count));

    auto vk_command_buffers = m_device->allocateVkCommandBuffers(allocate_info);

    auto command_buffers = std::vector<render::CommandBufferOwnedPtr> {};
    command_buffers.reserve(std::size(vk_command_buffers));

    for (auto &vk_command_buffer : vk_command_buffers)
        command_buffers.emplace_back(
            std::make_unique<CommandBuffer>(*this, level, std::move(vk_command_buffer)));

    return command_buffers;
}

/////////////////////////////////////
/////////////////////////////////////
vk::Result Queue::vkPresent(const vk::PresentInfoKHR &present_info) const noexcept {
    return m_vk_queue.presentKHR(present_info, m_device->vkDispatcher());
}
