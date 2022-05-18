// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.mpp>

#include <stormkit/gpu/resource/Shader.mpp>

#include <stormkit/gpu/pipeline/ComputePipeline.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    ComputePipeline::ComputePipeline(const Device &device, const PipelineCache *cache) noexcept
        : AbstractPipeline { device, cache } {}

    /////////////////////////////////////
    /////////////////////////////////////
    ComputePipeline::~ComputePipeline() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    ComputePipeline::ComputePipeline(ComputePipeline &&other) noexcept
        : AbstractPipeline { std::move(other) }, m_state { std::exchange(other.m_state, {}) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto ComputePipeline::operator=(ComputePipeline &&other) noexcept -> ComputePipeline & {
        if (&other == this) [[unlikely]]
            return *this;

        AbstractPipeline::operator=(std::move(other));

        m_state = std::exchange(other.m_state, {});

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ComputePipeline::bake() -> void {
        STORMKIT_EXPECTS(m_is_baked == false);

        const auto &vk = device().table();

        const auto vk_shader = VkPipelineShaderStageCreateInfo {
            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage  = core::as<VkShaderStageFlagBits>(m_state.shader_state.shader->type()),
            .module = *m_state.shader_state.shader,
            .pName  = "main"
        };

        const auto set_layout = core::transform(m_state.layout.descriptor_set_layouts,
                                                getHandle<const DescriptorSetLayout *>);

        const auto push_constant_ranges =
            core::transform(m_state.layout.push_constant_ranges, [](const auto &r) {
                return VkPushConstantRange {
                    .stageFlags = core::as<VkQueueFlags>(r.stages),
                    .offset     = r.offset,
                    .size       = core::as<core::UInt32>(r.size),
                };
            });

        const auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount         = core::as<core::UInt32>(std::size(set_layout)),
            .pSetLayouts            = std::data(set_layout),
            .pushConstantRangeCount = core::as<core::UInt32>(std::size(push_constant_ranges)),
            .pPushConstantRanges    = std::data(push_constant_ranges)
        };

        CHECK_VK_ERROR(vk.vkCreatePipelineLayout(device(),
                                                 &pipeline_layout_create_info,
                                                 nullptr,
                                                 &m_pipeline_layout));

        const auto create_info =
            VkComputePipelineCreateInfo { .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                                          .stage  = vk_shader,
                                          .layout = m_pipeline_layout,
                                          .basePipelineHandle = VK_NULL_HANDLE,
                                          .basePipelineIndex  = -1 };

        const auto pipeline_cache = (m_pipeline_cache != nullptr)
                                        ? m_pipeline_cache->vkHandle()
                                        : VkPipelineCache { VK_NULL_HANDLE };

        CHECK_VK_ERROR(vk.vkCreateComputePipelines(device(),
                                                   pipeline_cache,
                                                   1,
                                                   &create_info,
                                                   nullptr,
                                                   &m_pipeline));

        m_is_baked = true;
    }
} // namespace stormkit::gpu
