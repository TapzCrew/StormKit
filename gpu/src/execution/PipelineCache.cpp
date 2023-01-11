// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :PipelineCache;
import :Pipeline;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
    #include <stormkit/Log.hpp>
#endif

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
        if (m_pipeline_cache != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            saveCache();

            vk.vkDestroyPipelineCache(device(), m_pipeline_cache, nullptr);
        }
    };

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::PipelineCache(PipelineCache&& other) noexcept
        : DeviceObject { std::move(other) }, m_path { std::exchange(other.m_path, {}) },
          m_pipeline_cache { std::exchange(other.m_pipeline_cache, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_path           = std::exchange(other.m_path, {});
        m_pipeline_cache = std::exchange(other.m_pipeline_cache, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::createNewPipelineCache() -> void {
        const auto& vk = device().table();

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

        const auto create_info =
            VkPipelineCacheCreateInfo { .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };

        CHECK_VK_ERROR(
            vk.vkCreatePipelineCache(device(), &create_info, nullptr, &m_pipeline_cache));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::readPipelineCache() -> void {
        const auto& vk = device().table();

        const auto physical_device_infos = device().physicalDevice().info();

        pipeline_cache_logger.ilog("Loading pipeline cache {}", m_path.string());

        auto stream = std::ifstream { m_path.string(), std::ios::binary };
        core::read(stream, core::toByteSpan(m_serialized.guard));
        core::read(stream, core::toByteSpan(m_serialized.infos));
        core::read(stream, core::toByteSpan(m_serialized.uuid.value));

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

        if (!std::equal(std::cbegin(m_serialized.uuid.value),
                        std::cend(m_serialized.uuid.value),
                        std::cbegin(physical_device_infos.pipeline_cache_uuid))) {
            pipeline_cache_logger.elog("Mismatch pipeline cache device UUID");

            createNewPipelineCache();
            return;
        }

        const auto data = core::read(stream, m_serialized.guard.data_size);

        const auto create_info =
            VkPipelineCacheCreateInfo { .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
                                        .initialDataSize = std::size(data),
                                        .pInitialData    = std::data(data) };

        CHECK_VK_ERROR(
            vk.vkCreatePipelineCache(device(), &create_info, nullptr, &m_pipeline_cache));
    } // namespace stormkit::gpu

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::saveCache() -> void {
        const auto& vk = device().table();

        const auto data = [&] {
            auto size = core::RangeExtent { 0 };
            CHECK_VK_ERROR(vk.vkGetPipelineCacheData(device(), m_pipeline_cache, &size, nullptr));
            auto d = std::vector<core::Byte> { size };
            CHECK_VK_ERROR(
                vk.vkGetPipelineCacheData(device(), m_pipeline_cache, &size, std::data(d)));

            return d;
        }();

        pipeline_cache_logger.ilog("Saving pipeline cache at {}", m_path.string());

        m_serialized.guard.data_size = std::size(data);
        m_serialized.guard.data_hash = 0u;

        for (auto v : data) core::hashCombine(m_serialized.guard.data_hash, v);

        auto stream = std::ofstream { m_path.string(), std::ios::binary | std::ios::trunc };
        core::write(stream, core::asByteView(m_serialized.guard));
        core::write(stream, core::asByteView(m_serialized.infos));
        core::write(stream, core::asByteView(m_serialized.uuid.value));
        core::write(stream, core::asByteView(data));
    }
} // namespace stormkit::gpu
