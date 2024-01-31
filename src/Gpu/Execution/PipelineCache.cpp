// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

#include <stormkit/Log/LogMacro.hpp>

import :Vulkan;
import :Core;
import :Execution.Pipeline;

namespace stormkit::gpu {
    LOGGER("stormkit.Gpu")

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::createNewPipelineCache() -> VulkanExpected<void> {
        const auto physical_device_infos = device().physicalDevice().info();

        m_serialized.guard.magic     = MAGIC;
        m_serialized.guard.data_size = 0u;
        m_serialized.guard.data_hash = 0u;

        m_serialized.infos.version   = VERSION;
        m_serialized.infos.vendor_id = physical_device_infos.vendor_id;
        m_serialized.infos.device_id = physical_device_infos.device_id;

        std::ranges::copy(physical_device_infos.pipeline_cache_uuid,
                          std::ranges::begin(m_serialized.uuid.value));

        const auto create_info = vk::PipelineCacheCreateInfo {};

        return vkCreate<vk::raii::PipelineCache>(this->device().vkHandle(), create_info)
            .transform(core::monadic::set(m_vk_pipeline_cache))
            .transform([this] noexcept -> void {
                ilog("Created new pipeline cache at {}", m_path.string());
            });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::readPipelineCache() -> VulkanExpected<void> {
        const auto physical_device_infos = device().physicalDevice().info();

        auto stream = std::ifstream { m_path.string(), std::ios::binary };
        core::read(stream, core::asByteView(m_serialized.guard));
        core::read(stream, core::asByteView(m_serialized.infos));
        core::read(stream, core::asByteView(m_serialized.uuid.value));

        if (m_serialized.guard.magic != MAGIC) {
            elog("Invalid pipeline cache magic number, have {}, expected: {}",
                 m_serialized.guard.magic,
                 MAGIC);

            return createNewPipelineCache();
        }

        if (m_serialized.infos.version != VERSION) {
            elog("Mismatch pipeline cache version, have {}, expected: {}",
                 m_serialized.infos.version,
                 VERSION);

            return createNewPipelineCache();
        }

        if (m_serialized.infos.vendor_id != physical_device_infos.vendor_id) {
            elog("Mismatch pipeline cache vendor id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.vendor_id,
                 physical_device_infos.vendor_id);

            return createNewPipelineCache();
        }

        if (m_serialized.infos.device_id != physical_device_infos.device_id) {
            elog("Mismatch pipeline cache device id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.device_id,
                 physical_device_infos.device_id);

            return createNewPipelineCache();
        }

        if (!std::equal(std::cbegin(m_serialized.uuid.value),
                        std::cend(m_serialized.uuid.value),
                        std::cbegin(physical_device_infos.pipeline_cache_uuid))) {
            elog("Mismatch pipeline cache device UUID");

            return createNewPipelineCache();
        }

        const auto data = core::read(stream, m_serialized.guard.data_size);

        const auto create_info = vk::PipelineCacheCreateInfo {}.setInitialData<core::Byte>(data);

        return vkCreate<vk::raii::PipelineCache>(this->device().vkHandle(), create_info)
            .transform(core::monadic::set(m_vk_pipeline_cache))
            .transform([this]() noexcept -> void {
                ilog("Loading pipeline cache {}", m_path.string());
                return;
            });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::saveCache() -> void {
        vkCall(*m_vk_pipeline_cache, &vk::raii::PipelineCache::getData)
            .transform([this](auto&& data) noexcept {
                m_serialized.guard.data_size = std::size(data);
                m_serialized.guard.data_hash = 0u;

                for (auto v : data) core::hashCombine(m_serialized.guard.data_hash, v);

                auto stream = std::ofstream { m_path.string(), std::ios::binary | std::ios::trunc };
                core::write(stream, core::asByteView(m_serialized.guard));

                core::write(stream, core::asByteView(m_serialized.infos));
                core::write(stream, core::asByteView(m_serialized.uuid.value));
                core::write(stream, core::asByteView(data));
                ilog("Saving pipeline cache at {}", m_path.string());
            })
            .transform_error([this](auto&& error) noexcept {
                ilog("Failed to save pipeline cache at {}, reason: {}", m_path.string(), error);
                return error;
            });
    }
} // namespace stormkit::gpu
