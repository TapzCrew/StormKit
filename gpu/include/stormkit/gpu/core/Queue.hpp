// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <chrono>
#include <optional>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/sync/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Queue: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Queue;

        Queue(const gpu::Device &device, QueueFlag flags, core::UInt32 family_index, VkQueue queue);
        ~Queue();

        Queue(const Queue &) = delete;
        auto operator=(const Queue &) noexcept = delete;

        Queue(Queue &&) noexcept;
        auto operator=(Queue &&) noexcept -> Queue &;

        auto waitIdle() const noexcept -> void;

        auto submit(std::span<const CommandBuffer *> command_buffers,
                    std::span<const Semaphore *> wait_semaphores   = {},
                    std::span<const Semaphore *> signal_semaphores = {},
                    Fence *fence = nullptr) const noexcept -> void;

        [[nodiscard]] auto
            createCommandBuffer(CommandBufferLevel level = CommandBufferLevel::Primary) const
            -> CommandBuffer;
        [[nodiscard]] auto
            allocateCommandBuffer(CommandBufferLevel level = CommandBufferLevel::Primary) const
            -> CommandBufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedCommandBuffer(
            CommandBufferLevel level = CommandBufferLevel::Primary) const -> CommandBufferSharedPtr;

        [[nodiscard]] auto
            createCommandBuffers(core::USize count,
                                 CommandBufferLevel level = CommandBufferLevel::Primary) const
            -> std::vector<CommandBuffer>;
        [[nodiscard]] auto
            allocateCommandBuffers(core::USize count,
                                   CommandBufferLevel level = CommandBufferLevel::Primary) const
            -> std::vector<CommandBufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedCommandBuffers(
            core::USize count,
            CommandBufferLevel level = CommandBufferLevel::Primary) const
            -> std::vector<CommandBufferSharedPtr>;

        [[nodiscard]] auto type() const noexcept -> QueueFlag;
        [[nodiscard]] auto familyIndex() const noexcept -> core::UInt32;

        [[nodiscard]] operator VkQueue() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkQueue;
        [[nodiscard]] auto vkCommandPool() const noexcept -> VkCommandPool;

      private:
        auto deleteCommandBuffer(VkCommandBuffer cmb) const noexcept -> void;

        auto createVkCommandBuffers(core::USize count, gpu::CommandBufferLevel level) const noexcept
            -> std::vector<VkCommandBuffer>;

        QueueFlag m_queue_flag = QueueFlag::None;

        core::UInt32 m_family_index = 0;

        VkQueue m_queue = VK_NULL_HANDLE;

        VkCommandPool m_command_pool = VK_NULL_HANDLE;
    };

} // namespace stormkit::gpu

#if defined(STORMKIT_CXX20_MODULES)
}
#endif

#include "Queue.inl"
