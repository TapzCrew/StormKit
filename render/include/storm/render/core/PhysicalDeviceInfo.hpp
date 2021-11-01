// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <string>

#include <storm/core/Strings.hpp>

#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct PhysicalDeviceInfo {
        core::UInt64 device_id;
        std::string device_name;
        core::UInt64 vendor_id;
        std::string vendor_name;

        core::UInt32 api_major_version;
        core::UInt32 api_minor_version;
        core::UInt32 api_patch_version;

        core::UInt32 driver_major_version;
        core::UInt32 driver_minor_version;
        core::UInt32 driver_patch_version;

        std::array<core::UInt8, VK_UUID_SIZE> pipeline_cache_uuid;

        PhysicalDeviceType type;
    };
FORMATTER(storm::render::PhysicalDeviceInfo,
              "PhysicalDeviceInfo {{\n"
              "   device_id: {:#06x},\n"
              "   device_name: {},\n"
              "   vendor_id: {:#06x},\n"
              "   vendor_name: {},\n"
              "   api_version: {}.{}.{},\n"
              "   driver_version: {}.{}.{},\n"
              "   type: {}\n"
              "}}",
              data.device_id,
              data.device_name,
              data.vendor_id,
              data.vendor_name,
              data.api_major_version,
              data.api_minor_version,
              data.api_patch_version,
              data.driver_major_version,
              data.driver_minor_version,
              data.driver_patch_version,
              storm::render::to_string(data.type));
} // namespace storm::render

CUSTOM_FORMAT(storm::render::PhysicalDeviceInfo, storm::render::toString(data))
