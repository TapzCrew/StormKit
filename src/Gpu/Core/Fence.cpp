// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import vulkan;

import stormkit.Core;

import <stormkit/Gpu/Core/VulkanMacro.hpp>;

import :Core.Sync;
import :Core.Device;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(const Device& device, bool signaled) : DeviceObject { device } {
        const auto flags =
            (signaled) ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags {};

        const auto create_info = vk::FenceCreateInfo {}.setFlags(flags);

        try {
            m_vk_fence = vk::raii::Fence { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::~Fence() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::Fence(Fence&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::operator=(Fence&& other) noexcept -> Fence& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Fence::status() const noexcept -> Status {
        const auto status = m_vk_fence->getStatus();
        if (status == vk::Result::eSuccess) return Status::Signaled;
        else if (status == vk::Result::eNotReady)
            return Status::Unsignaled;

        CHECK_VK_ERROR(status);

        std::unreachable();

        return Status::Unsignaled; // can't go here
    }
} // namespace stormkit::gpu
