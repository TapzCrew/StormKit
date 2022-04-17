// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Instance::physicalDevices() -> std::span<const PhysicalDevice> {
        return m_physical_devices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Instance::loader() const noexcept -> const DynamicLoader & { return m_loader; }
    /////////////////////////////////////
    /////////////////////////////////////
    inline Instance::operator VkInstance() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Instance::vkHandle() const noexcept -> VkInstance { return m_instance; }
} // namespace stormkit::render
