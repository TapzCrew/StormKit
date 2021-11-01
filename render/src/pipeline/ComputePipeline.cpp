// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/ComputePipeline.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
ComputePipeline::ComputePipeline(const render::Device &device, PipelineCacheConstPtr cache) noexcept
    : AbstractPipeline { device, cache } {
}

/////////////////////////////////////
/////////////////////////////////////
ComputePipeline::~ComputePipeline() = default;

/////////////////////////////////////
/////////////////////////////////////
ComputePipeline::ComputePipeline(ComputePipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
ComputePipeline &ComputePipeline::operator=(ComputePipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void ComputePipeline::build() {
    STORMKIT_EXPECTS(m_is_builded == false);

    const auto &device = static_cast<const Device &>(*m_device);

    const auto vk_shader = vk::PipelineShaderStageCreateInfo {}
                               .setStage(toVKBits(m_state.shader_state.shader->type()))
                               .setModule(*m_state.shader_state.shader)
                               .setPName("main");

    auto set_layout = std::vector<vk::DescriptorSetLayout> {};
    set_layout.reserve(std::size(m_state.layout.descriptor_set_layouts));
    for (const auto &layout : m_state.layout.descriptor_set_layouts)
        set_layout.emplace_back(*layout);

    auto push_constant_ranges = std::vector<vk::PushConstantRange> {};
    push_constant_ranges.reserve(std::size(m_state.layout.push_constant_ranges));
    for (const auto &push_constant_range : m_state.layout.push_constant_ranges) {
        const auto vk_push_constant_range = vk::PushConstantRange {}
                                                .setStageFlags(toVK(push_constant_range.stages))
                                                .setOffset(push_constant_range.offset)
                                                .setSize(push_constant_range.size);

        push_constant_ranges.emplace_back(std::move(vk_push_constant_range));
    }

    const auto pipeline_layout_create_info =
        vk::PipelineLayoutCreateInfo {}
            .setSetLayoutCount(gsl::narrow_cast<core::Int32>(std::size(set_layout)))
            .setPSetLayouts(std::data(set_layout))
            .setPushConstantRangeCount(
                gsl::narrow_cast<core::Int32>(std::size(push_constant_ranges)))
            .setPPushConstantRanges(std::data(push_constant_ranges));

    m_vk_pipeline_layout = m_device->createVkPipelineLayout(pipeline_layout_create_info);

    const auto create_info = vk::ComputePipelineCreateInfo {}
                                 .setStage(vk_shader)
                                 .setLayout(*m_vk_pipeline_layout)
                                 .setBasePipelineHandle(VK_NULL_HANDLE)
                                 .setBasePipelineIndex(-1);

    if (m_pipeline_cache)
        m_vk_pipeline = device.createVkComputePipeline(create_info, m_pipeline_cache->vkHandle());
    else
        m_vk_pipeline = device.createVkComputePipeline(create_info);

    m_is_builded = true;
}
