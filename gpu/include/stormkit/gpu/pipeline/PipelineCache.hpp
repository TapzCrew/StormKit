// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>

#include <stormkit/core/HashMap.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/ComputePipeline.hpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>
#include <stormkit/gpu/pipeline/GraphicsPipeline.hpp>
#include <stormkit/gpu/pipeline/RenderPassDescription.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC PipelineCache: public DeviceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Pipeline_Cache;

        explicit PipelineCache(const Device &device,
                               std::filesystem::path cache_path = "./pipeline_cache.bin");
        ~PipelineCache();

        PipelineCache(const PipelineCache &) = delete;
        auto operator=(const PipelineCache &) -> PipelineCache & = delete;

        PipelineCache(PipelineCache &&) noexcept;
        auto operator=(PipelineCache &&) noexcept -> PipelineCache &;

        [[nodiscard]] auto getPipeline(const GraphicsPipelineState &state,
                                       const RenderPass &render_pass) -> GraphicsPipeline &;
        [[nodiscard]] auto getPipeline(const ComputePipelineState &state) -> ComputePipeline &;

        [[nodiscard]] operator VkPipelineCache() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkPipelineCache;

      private:
        auto has(const GraphicsPipelineState &state,
                 const RenderPassDescription &description) const noexcept -> bool;
        auto has(const ComputePipelineState &state) const noexcept -> bool;

        auto createNewPipelineCache() -> void;
        auto readPipelineCache() -> void;
        auto saveCache() -> void;

        static constexpr auto MAGIC   = core::UInt32 { 0xDEADBEEF };
        static constexpr auto VERSION = core::UInt32 { 1u };
        struct SerializedCache {
            struct {
                core::UInt32 magic;
                core::USize data_size;
                core::UInt64 data_hash;
            } guard;

            struct {
                core::UInt32 version;
                core::UInt64 vendor_id;
                core::UInt64 device_id;
            } infos;

            struct {
                std::array<core::UInt8, VK_UUID_SIZE> value;
            } uuid;
        } m_serialized;

        std::filesystem::path m_path = {};

        VkPipelineCache m_pipeline_cache = VK_NULL_HANDLE;

        core::HashMap<GraphicsPipelineState, core::HashMap<RenderPassDescription, GraphicsPipeline>>
            m_graphics_pipelines                                                 = {};
        core::HashMap<ComputePipelineState, ComputePipeline> m_compute_pipelines = {};
    };
} // namespace stormkit::gpu

#include "PipelineCache.inl"
