// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <initializer_list>
#include <optional>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>

#include <storm/render/sync/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Queue: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Queue;

        Queue(const render::Device &device,
              QueueFlag flags,
              core::UInt32 family_index,
              vk::Queue queue);
        ~Queue();

        Queue(Queue &&);
        Queue &operator=(Queue &&);

        void waitIdle() const noexcept;

        void submit( // todo optimise rvalue / lvalue
            storm::core::span<const CommandBufferConstPtr> command_buffers,
            storm::core::span<const SemaphoreConstPtr> wait_semaphores   = {},
            storm::core::span<const SemaphoreConstPtr> signal_semaphores = {},
            FencePtr fence                                               = nullptr) const noexcept;

        CommandBuffer
            createCommandBuffer(CommandBufferLevel level = CommandBufferLevel::Primary) const;
        CommandBufferOwnedPtr
            createCommandBufferPtr(CommandBufferLevel level = CommandBufferLevel::Primary) const;

        std::vector<CommandBuffer>
            createCommandBuffers(core::ArraySize count,
                                 CommandBufferLevel level = CommandBufferLevel::Primary) const;
        std::vector<CommandBufferOwnedPtr>
            createCommandBufferPtrs(core::ArraySize count,
                                    CommandBufferLevel level = CommandBufferLevel::Primary) const;

        vk::Result vkPresent(const vk::PresentInfoKHR &present_info) const noexcept;

        inline QueueFlag type() const noexcept;
        inline const Device &device() const noexcept;
        inline core::UInt32 familyIndex() const noexcept;

        inline vk::Queue vkQueue() const noexcept;
        inline operator vk::Queue() const noexcept;
        inline vk::Queue vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

        inline vk::CommandPool vkCommandPool() const noexcept;

      private:
        DeviceConstPtr m_device;

        QueueFlag m_queue_flag;

        core::UInt32 m_family_index;

        vk::Queue m_vk_queue = VK_NULL_HANDLE;

        RAIIVkCommandPool m_vk_command_pool;
    };

} // namespace storm::render

#include "Queue.inl"
