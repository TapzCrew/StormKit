// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import vulkan;

import stormkit.Core;

import :Core.Sync;
import :Core.Device;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(const Device& device) : DeviceObject { device } {
        const auto create_info = vk::SemaphoreCreateInfo {};

        try {
            m_vk_semaphore = vk::raii::Semaphore(this->device().vkHandle(), create_info);
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::~Semaphore() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::Semaphore(Semaphore&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Semaphore::operator=(Semaphore&& other) noexcept -> Semaphore& = default;
} // namespace stormkit::gpu
