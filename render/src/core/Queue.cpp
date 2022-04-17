
// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <array>

#include <stormkit/render/core/CommandBuffer.mpp>
#include <stormkit/render/core/Device.mpp>
#include <stormkit/render/core/Queue.mpp>
#include <stormkit/render/sync/Fence.mpp>
#include <stormkit/render/sync/Semaphore.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(const render::Device &device,
                 render::QueueFlag flags,
                 core::UInt32 family_index,
                 VkQueue queue)
        : m_device { &device }, m_queue_flag { flags }, m_family_index { family_index }, m_queue {
              queue
          } {
        const auto create_info =
            VkCommandPoolCreateInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                      .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                                               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT };

        const auto &vk = m_device->table();

        CHECK_VK_ERROR(vk.vkCreateCommandPool(*m_device, &create_info, nullptr, &m_command_pool));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::~Queue() {
        if (m_command_pool != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyCommandPool(device(), m_command_pool, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::Queue(Queue &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_queue_flag { std::exchange(other.m_queue_flag, QueueFlag::None) },
          m_family_index { std::exchange(other.m_family_index, 0) },
          m_queue { std::exchange(other.m_queue, VK_NULL_HANDLE) }, m_command_pool {
              std::exchange(other.m_command_pool, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::operator=(Queue &&other) noexcept -> Queue & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device       = std::exchange(other.m_device, nullptr);
        m_queue_flag   = std::exchange(other.m_queue_flag, QueueFlag::None);
        m_family_index = std::exchange(other.m_family_index, 0);
        m_queue        = std::exchange(other.m_queue, VK_NULL_HANDLE);
        m_command_pool = std::exchange(other.m_command_pool, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::waitIdle() const noexcept -> void {
        STORMKIT_EXPECTS(m_queue);

        const auto &vk = device().table();

        CHECK_VK_ERROR(vk.vkQueueWaitIdle(m_queue));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::submit(std::span<const CommandBuffer *> command_buffers,
                       std::span<const Semaphore *> wait_semaphores,
                       std::span<const Semaphore *> signal_semaphores,
                       Fence *fence) const noexcept -> void {
        const auto &vk = device().table();

        auto vk_command_buffers = std::vector<VkCommandBuffer> {};
        vk_command_buffers.reserve(std::size(command_buffers));
        std::ranges::transform(command_buffers,
                               std::back_inserter(vk_command_buffers),
                               [](const auto &elem) { return elem->vkHandle(); });

        auto vk_wait_semaphores = std::vector<VkSemaphore> {};
        vk_wait_semaphores.reserve(std::size(wait_semaphores));
        std::ranges::transform(wait_semaphores,
                               std::back_inserter(vk_wait_semaphores),
                               [](const auto &elem) { return elem->vkHandle(); });

        auto vk_signal_semaphores = std::vector<VkSemaphore> {};
        vk_signal_semaphores.reserve(std::size(signal_semaphores));
        std::ranges::transform(command_buffers,
                               std::back_inserter(vk_command_buffers),
                               [](const auto &elem) { return elem->vkHandle(); });

        auto wait_stages = std::vector<VkPipelineStageFlags> {};
        wait_stages.resize(core::as<core::USize>(std::size(wait_semaphores)),
                           VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        const auto submit_info = VkSubmitInfo {
            .waitSemaphoreCount   = core::as<core::UInt32>(std::size(vk_wait_semaphores)),
            .pWaitSemaphores      = std::data(vk_wait_semaphores),
            .pWaitDstStageMask    = std::data(wait_stages),
            .commandBufferCount   = core::as<core::UInt32>(std::size(vk_command_buffers)),
            .pCommandBuffers      = std::data(vk_command_buffers),
            .signalSemaphoreCount = core::as<core::UInt32>(std::size(vk_signal_semaphores)),
            .pSignalSemaphores    = std::data(vk_signal_semaphores)
        };

        auto vk_fence = VkFence { VK_NULL_HANDLE };
        if (fence) {
            fence->reset();

            vk_fence = *fence;
        }

        const auto submit_infos = std::array { submit_info };
        CHECK_VK_ERROR(
            vk.vkQueueSubmit(m_queue, std::size(submit_infos), std::data(submit_infos), vk_fence));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::createCommandBuffer(CommandBufferLevel level) const -> CommandBuffer {
        auto command_buffer = std::move(createCommandBuffers(1, level)[0]);

        return command_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::allocateCommandBuffer(CommandBufferLevel level) const -> CommandBufferOwnedPtr {
        auto command_buffer = std::move(allocateCommandBuffers(1, level)[0]);

        return command_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::allocateRefCountedCommandBuffer(CommandBufferLevel level) const
        -> CommandBufferSharedPtr {
        auto command_buffer = std::move(allocateRefCountedCommandBuffers(1, level)[0]);

        return command_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::createCommandBuffers(core::USize count, render::CommandBufferLevel level) const
        -> std::vector<render::CommandBuffer> {
        auto vk_command_buffers = createVkCommandBuffers(count, level);

        auto command_buffers = std::vector<CommandBuffer> {};
        command_buffers.reserve(std::size(vk_command_buffers));

        std::ranges::transform(vk_command_buffers,
                               std::back_inserter(command_buffers),
                               [level, this](const auto &elem) {
                                   return CommandBuffer { *this,
                                                          level,
                                                          elem,
                                                          [this](auto cmb) {
                                                              deleteCommandBuffer(cmb);
                                                          },
                                                          CommandBuffer::Tag {} };
                               });

        return command_buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::allocateCommandBuffers(core::USize count, render::CommandBufferLevel level) const
        -> std::vector<render::CommandBufferOwnedPtr> {
        auto vk_command_buffers = createVkCommandBuffers(count, level);

        auto command_buffers = std::vector<CommandBufferOwnedPtr> {};
        command_buffers.reserve(std::size(vk_command_buffers));

        std::ranges::transform(vk_command_buffers,
                               std::back_inserter(command_buffers),
                               [level, this](const auto &elem) {
                                   return std::make_unique<CommandBuffer>(
                                       *this,
                                       level,
                                       elem,
                                       [this](auto cmb) { deleteCommandBuffer(cmb); },
                                       CommandBuffer::Tag {});
                               });

        return command_buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::allocateRefCountedCommandBuffers(core::USize count,
                                                 render::CommandBufferLevel level) const
        -> std::vector<render::CommandBufferSharedPtr> {
        auto vk_command_buffers = createVkCommandBuffers(count, level);

        auto command_buffers = std::vector<CommandBufferSharedPtr> {};
        command_buffers.reserve(std::size(vk_command_buffers));

        std::ranges::transform(vk_command_buffers,
                               std::back_inserter(command_buffers),
                               [level, this](const auto &elem) {
                                   return std::make_shared<CommandBuffer>(
                                       *this,
                                       level,
                                       elem,
                                       [this](auto cmb) { deleteCommandBuffer(cmb); },
                                       CommandBuffer::Tag {});
                               });

        return command_buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::deleteCommandBuffer(VkCommandBuffer cmb) const noexcept -> void {
        const auto &vk = device().table();

        vk.vkFreeCommandBuffers(device(), m_command_pool, 1, &cmb);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Queue::createVkCommandBuffers(core::USize count,
                                       render::CommandBufferLevel level) const noexcept
        -> std::vector<VkCommandBuffer> {
        const auto &vk = device().table();

        const auto allocate_info =
            VkCommandBufferAllocateInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                          .commandPool = m_command_pool,
                                          .level       = core::as<VkCommandBufferLevel>(level),
                                          .commandBufferCount = core::as<core::UInt32>(count) };

        auto vk_command_buffers = std::vector<VkCommandBuffer> {};
        vk_command_buffers.resize(count, nullptr);
        CHECK_VK_ERROR(
            vk.vkAllocateCommandBuffers(device(), &allocate_info, std::data(vk_command_buffers)));

        return vk_command_buffers;
    }
} // namespace stormkit::render
