// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

import <stormkit/Log/LogMacro.hpp>;

import :Execution.Pipeline;

import vulkan;

namespace stormkit::gpu {
    NAMED_LOGGER(pipeline_cache_logger, "stormkit.Gpu:core.PipelineCache")

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::PipelineCache(const Device& device, std::filesystem::path path)
        : DeviceObject { device }, m_path { std::move(path) } {
        if (std::filesystem::exists(m_path)) readPipelineCache();
        else
            createNewPipelineCache();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::~PipelineCache() {
        saveCache();
    };

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::PipelineCache(PipelineCache&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::createNewPipelineCache() -> void {
        pipeline_cache_logger.ilog("Creating new pipeline cache at {}", m_path.string());

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

        try {
            m_vk_pipeline_cache =
                vk::raii::PipelineCache { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::readPipelineCache() -> void {
        const auto physical_device_infos = device().physicalDevice().info();

        pipeline_cache_logger.ilog("Loading pipeline cache {}", m_path.string());

        auto stream = std::ifstream { m_path.string(), std::ios::binary };
        core::read(stream, core::asByteView(m_serialized.guard));
        core::read(stream, core::asByteView(m_serialized.infos));
        core::read(stream, core::asByteView(m_serialized.uuid.value));

        if (m_serialized.guard.magic != MAGIC) {
            pipeline_cache_logger.elog("Invalid pipeline cache magic number, have {}, expected: {}",
                                       m_serialized.guard.magic,
                                       MAGIC);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.version != VERSION) {
            pipeline_cache_logger.elog("Mismatch pipeline cache version, have {}, expected: {}",
                                       m_serialized.infos.version,
                                       VERSION);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.vendor_id != physical_device_infos.vendor_id) {
            pipeline_cache_logger
                .elog("Mismatch pipeline cache vendor id, have {:#06x}, expected: {:#06x}",
                      m_serialized.infos.vendor_id,
                      physical_device_infos.vendor_id);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.device_id != physical_device_infos.device_id) {
            pipeline_cache_logger
                .elog("Mismatch pipeline cache device id, have {:#06x}, expected: {:#06x}",
                      m_serialized.infos.device_id,
                      physical_device_infos.device_id);

            createNewPipelineCache();
            return;
        }

        if (not std::equal(std::cbegin(m_serialized.uuid.value),
                           std::cend(m_serialized.uuid.value),
                           std::cbegin(physical_device_infos.pipeline_cache_uuid))) {
            pipeline_cache_logger.elog("Mismatch pipeline cache device UUID");

            createNewPipelineCache();
            return;
        }

        const auto data = core::read(stream, m_serialized.guard.data_size);
        const auto create_info =
            vk::PipelineCacheCreateInfo {}.setInitialData(vk::ArrayProxyNoTemporaries(data));

        try {
            m_vk_pipeline_cache =
                vk::raii::PipelineCache { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    } // namespace stormkit::gpu

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::saveCache() -> void {
        const auto data = m_vk_pipeline_cache->getData();

        pipeline_cache_logger.ilog("Saving pipeline cache at {}", m_path.string());

        m_serialized.guard.data_size = std::size(data);
        m_serialized.guard.data_hash = core::hashCombine(data);

        auto stream = std::ofstream { m_path.string(), std::ios::binary | std::ios::trunc };
        core::write(stream, core::asByteView(m_serialized.guard));
        core::write(stream, core::asByteView(m_serialized.infos));
        core::write(stream, core::asByteView(m_serialized.uuid.value));
        core::write(stream, core::asByteView(data));
    }
} // namespace stormkit::gpu
