// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const DynamicLoader &Instance::loader() const noexcept { return m_loader; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::span<const PhysicalDeviceOwnedPtr> Instance::physicalDevices() {
        return m_physical_devices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Instance Instance::vkInstance() const noexcept {
        auto instance = vk::Instance { nullptr };

        if (std::holds_alternative<vk::UniqueInstance>(m_vk_instance))
            instance = *std::get<vk::UniqueInstance>(m_vk_instance);
        else
            instance = std::get<vk::Instance>(m_vk_instance);

        STORMKIT_EXPECTS(instance);

        return instance;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::operator vk::Instance() const noexcept { return vkInstance(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Instance Instance::vkHandle() const noexcept { return vkInstance(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Instance::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkInstance_T *());
    }
} // namespace storm::render
