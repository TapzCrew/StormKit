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
    CommandBufferPool::CommandBufferPool(const Device& device, const Queue& queue)
        : DeviceObject { device }, m_queue { &queue } {
        const auto create_info = vk::CommandPoolCreateInfo {}.setFlags(
            vk::CommandPoolBits::eCreateTransient | vk::CommandPoolBits::eCreateResetCommandBuffer);

        const auto& vk = this->device().table();

        CHECK_VK_ERROR(
            vk.vkCreateCommandPool(this->device(), &create_info, nullptr, &m_command_pool));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBufferPool::~CommandBufferPool() {
        if (m_command_pool != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyCommandPool(device(), m_command_pool, nullptr);

            m_command_pool = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    CommandBufferPool::CommandBufferPool(CommandBufferPool&& other) noexcept
        : DeviceObject { std::move(other) }, m_queue { std::exchange(other.m_queue, nullptr) },
          m_command_pool { std::exchange(other.m_command_pool, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBufferPool::operator=(CommandBufferPool&& other) noexcept -> CommandBufferPool& {
        if (&other == this) [[unlikely]]
            return *this;

        m_queue        = std::exchange(other.m_queue, nullptr);
        m_command_pool = std::exchange(other.m_command_pool, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBufferPool::deleteCommandBuffer(VkCommandBuffer cmb) const noexcept -> void {
        const auto& vk = device().table();

        vk.vkFreeCommandBuffers(device(), m_command_pool, 1, &cmb);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandBufferPool::createVkCommandBuffers(core::RangeExtent count,
                                                   CommandBufferLevel level) const noexcept
        -> std::vector<VkCommandBuffer> {
        const auto& vk = device().table();

        const auto allocate_info =
            VkCommandBufferAllocateInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                          .commandPool = m_command_pool,
                                          .level       = core::as<VkCommandBufferLevel>(level),
                                          .commandBufferCount = core::as<core::UInt32>(count) };

        auto vk_commandbuffers = std::vector<VkCommandBuffer> {};
        vk_commandbuffers.resize(count, nullptr);
        CHECK_VK_ERROR(
            vk.vkAllocateCommandBuffers(device(), &allocate_info, std::data(vk_commandbuffers)));

        return vk_commandbuffers;
    }
} // namespace stormkit::gpu
