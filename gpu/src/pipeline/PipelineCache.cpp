// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/Fstream.mpp>

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>

#include <stormkit/gpu/pipeline/PipelineCache.mpp>
#include <stormkit/gpu/pipeline/RenderPass.mpp>

#include "../Log.mpp"
#include "stormkit/core/Types.mpp"

namespace stormkit::gpu {

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
          m_pipeline_cache { std::exchange(other.m_pipeline_cache, VK_NULL_HANDLE) },
          m_graphics_pipelines { std::move(other.m_graphics_pipelines) }, m_compute_pipelines {
              std::move(other.m_compute_pipelines)
          } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::operator=(PipelineCache&& other) noexcept -> PipelineCache& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_path               = std::exchange(other.m_path, {});
        m_pipeline_cache     = std::exchange(other.m_pipeline_cache, VK_NULL_HANDLE);
        m_graphics_pipelines = std::move(other.m_graphics_pipelines);
        m_compute_pipelines  = std::move(other.m_compute_pipelines);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::getPipeline(const GraphicsPipelineState& state, const RenderPass& pass)
        -> GraphicsPipeline& {
        const auto pass_description = pass.description();
        if (!has(state, pass_description)) {
            auto pipeline = device().createGraphicsPipeline(this);

            pipeline.setState(state);
            pipeline.setRenderPass(pass);
            pipeline.bake();

            m_graphics_pipelines[state].emplace(pass_description, std::move(pipeline));
        }

        return std::ranges::find_if(m_graphics_pipelines[state],
                                    [&pass_description](const auto& d) {
                                        return d.first.isCompatible(pass_description);
                                    })
            ->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::getPipeline(const ComputePipelineState& state) -> ComputePipeline& {
        if (!has(state)) {
            auto pipeline = device().createComputePipeline(core::makeConstObserver(this));

            pipeline.setState(state);
            pipeline.bake();

            m_compute_pipelines.emplace(state, std::move(pipeline));
        }
        return m_compute_pipelines.find(state)->second;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::has(const GraphicsPipelineState& state,
                            const RenderPassDescription& description) const noexcept -> bool {
        auto it = m_graphics_pipelines.find(state);
        // auto it = core::ranges::find(m_pipelines, state);
        if (it == std::cend(m_graphics_pipelines)) return false;

        const auto& lookup = it->second;

        auto it2 =
            std::find_if(std::cbegin(lookup), std::cend(lookup), [&description](const auto& d) {
                return d.first.isCompatible(description);
            });

        return it2 != std::cend(lookup);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::has(const ComputePipelineState& state) const noexcept -> bool {
        return m_compute_pipelines.find(state) != std::cend(m_compute_pipelines);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto PipelineCache::createNewPipelineCache() -> void {
        const auto& vk = device().table();

        ilog("Creating new pipeline cache at {}", m_path.string());

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

        ilog("Loading pipeline cache {}", m_path.string());

        auto stream = std::ifstream { m_path.string(), std::ios::binary };
        core::read(stream, core::toByteSpan(m_serialized.guard));
        core::read(stream, core::toByteSpan(m_serialized.infos));
        core::read(stream, core::toByteSpan(m_serialized.uuid.value));

        if (m_serialized.guard.magic != MAGIC) {
            elog("Invalid pipeline cache magic number, have {}, expected: {}",
                 m_serialized.guard.magic,
                 MAGIC);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.version != VERSION) {
            elog("Mismatch pipeline cache version, have {}, expected: {}",
                 m_serialized.infos.version,
                 VERSION);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.vendor_id != physical_device_infos.vendor_id) {
            elog("Mismatch pipeline cache vendor id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.vendor_id,
                 physical_device_infos.vendor_id);

            createNewPipelineCache();
            return;
        }

        if (m_serialized.infos.device_id != physical_device_infos.device_id) {
            elog("Mismatch pipeline cache device id, have {:#06x}, expected: {:#06x}",
                 m_serialized.infos.device_id,
                 physical_device_infos.device_id);

            createNewPipelineCache();
            return;
        }

        if (!std::equal(std::cbegin(m_serialized.uuid.value),
                        std::cend(m_serialized.uuid.value),
                        std::cbegin(physical_device_infos.pipeline_cache_uuid))) {
            elog("Mismatch pipeline cache device UUID");

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
            auto size = core::USize { 0 };
            CHECK_VK_ERROR(vk.vkGetPipelineCacheData(device(), m_pipeline_cache, &size, nullptr));
            auto d = core::ByteArray { size };
            CHECK_VK_ERROR(
                vk.vkGetPipelineCacheData(device(), m_pipeline_cache, &size, std::data(d)));

            return d;
        }();

        ilog("Saving pipeline cache at {}", m_path.string());

        m_serialized.guard.data_size = std::size(data);
        m_serialized.guard.data_hash = 0u;

        for (auto v : data) core::hashCombine(m_serialized.guard.data_hash, v);

        auto stream = std::ofstream { m_path.string(), std::ios::binary | std::ios::trunc };
        core::write(stream, core::toConstByteSpan(m_serialized.guard));
        core::write(stream, core::toConstByteSpan(m_serialized.infos));
        core::write(stream, core::toConstByteSpan(m_serialized.uuid.value));
        core::write(stream, core::toConstByteSpan(data));
    }
} // namespace stormkit::gpu
