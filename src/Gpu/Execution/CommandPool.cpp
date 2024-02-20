// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import stormkit.Gpu.Vulkan;
import :Core;
import :Execution.CommandBuffer;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    auto CommandPool::createVkCommandBuffers(core::RangeExtent count, CommandBufferLevel level)
        const noexcept -> std::vector<vk::raii::CommandBuffer> {
        auto out          = std::vector<vk::raii::CommandBuffer> {};
        auto reuse_count  = std::empty(m_reusable_command_buffers)
                                ? 0
                                : core::math::abs(std::size(m_reusable_command_buffers) - count);
        auto create_count = count - reuse_count;

        {
            // auto lock = std::unique_lock { m_reuse_mutex };
            // auto erase_end   = std::ranges::end(m_reusable_command_buffers);
            // auto erase_begin = std::ranges::end(m_reusable_command_buffers) - reuse_count;
            //
            // std::ranges::for_each(m_reusable_command_buffers | std::views::reverse |
            //                           std::views::take(reuse_count),
            //                       [&out](vk::raii::CommandBuffer&& cmb) {
            //                           out.emplace_back(std::move(cmb));
            //                       });
            //
            // m_reusable_command_buffers.erase(erase_begin, erase_end);
        }
        out.reserve(count);

        if (create_count > 0) {
            const auto allocate_info = vk::CommandBufferAllocateInfo {
                .commandPool        = *vkHandle(),
                .level              = core::narrow<vk::CommandBufferLevel>(level),
                .commandBufferCount = core::as<core::UInt32>(count)
            };

            // TODO handle error here
            core::moveAndMerge(out, device().vkHandle().allocateCommandBuffers(allocate_info).value());
        }

        return out;
    }
} // namespace stormkit::gpu