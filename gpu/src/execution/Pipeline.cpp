// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :Pipeline;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::Pipeline(const gpu::Device& device, const PipelineCache *cache) noexcept
        : DeviceObject { device }, m_pipeline_cache { cache } {};

    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::~Pipeline() {
        const auto& vk = device().table();

        if (m_pipeline_layout != VK_NULL_HANDLE) [[likely]]
            vk.vkDestroyPipelineLayout(device(), m_pipeline_layout, nullptr);

        if (m_pipeline != VK_NULL_HANDLE) [[likely]]
            vk.vkDestroyPipeline(device(), m_pipeline, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Pipeline::Pipeline(Pipeline&& other) noexcept
        : DeviceObject { std::move(other) },
          m_pipeline_cache { std::exchange(other.m_pipeline_cache, nullptr) },
          m_is_baked { std::exchange(other.m_is_baked, false) },
          m_pipeline { std::exchange(other.m_pipeline, VK_NULL_HANDLE) },
          m_pipeline_layout { std::exchange(other.m_pipeline_layout, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Pipeline::operator=(Pipeline&& other) noexcept -> Pipeline& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_pipeline_cache  = std::exchange(other.m_pipeline_cache, nullptr);
        m_is_baked        = std::exchange(other.m_is_baked, false);
        m_pipeline        = std::exchange(other.m_pipeline, VK_NULL_HANDLE);
        m_pipeline_layout = std::exchange(other.m_pipeline_layout, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
