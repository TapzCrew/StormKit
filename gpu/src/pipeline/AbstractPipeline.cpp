// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.mpp>

#include <stormkit/gpu/pipeline/AbstractPipeline.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    AbstractPipeline::AbstractPipeline(const gpu::Device &device,
                                       const PipelineCache *cache) noexcept
        : m_device { &device }, m_pipeline_cache { cache } {};

    /////////////////////////////////////
    /////////////////////////////////////
    AbstractPipeline::~AbstractPipeline() {
        const auto &vk = device().table();

        if (m_pipeline_layout != VK_NULL_HANDLE) [[likely]]
            vk.vkDestroyPipelineLayout(device(), m_pipeline_layout, nullptr);

        if (m_pipeline != VK_NULL_HANDLE) [[likely]]
            vk.vkDestroyPipeline(device(), m_pipeline, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    AbstractPipeline::AbstractPipeline(AbstractPipeline &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_pipeline_cache { std::exchange(other.m_pipeline_cache, nullptr) },
          m_is_baked { std::exchange(other.m_is_baked, false) },
          m_pipeline { std::exchange(other.m_pipeline, VK_NULL_HANDLE) }, m_pipeline_layout {
              std::exchange(other.m_pipeline_layout, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto AbstractPipeline::operator=(AbstractPipeline &&other) noexcept -> AbstractPipeline & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device          = std::exchange(other.m_device, nullptr);
        m_pipeline_cache  = std::exchange(other.m_pipeline_cache, nullptr);
        m_is_baked        = std::exchange(other.m_is_baked, false);
        m_pipeline        = std::exchange(other.m_pipeline, VK_NULL_HANDLE);
        m_pipeline_layout = std::exchange(other.m_pipeline_layout, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
